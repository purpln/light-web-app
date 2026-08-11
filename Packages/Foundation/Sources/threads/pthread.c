#if defined(__wasm__)

#include <malloc.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include "implementation.h"

#ifndef STACK_SIZE
#define STACK_SIZE 64 * 1024
#endif
static_assert(STACK_SIZE % 16 == 0, "Stack size must be a multiple of 16");

enum {
    THREAD_JOINABLE = 0,
    THREAD_DETACHED = 1,
    THREAD_EXITED = 2,
};

#define STACK_GUARD 0x51acc0deu

static _Thread_local struct opaque_pthread_t __pthread_self_internal;

nonnull pthread_t __pthread_self(void) {
    return (pthread_t)&__pthread_self_internal;
}

static inline void futex_wait(_Atomic int *address, int expected) {
    __builtin_wasm_memory_atomic_wait32((int *)address, expected, -1);
}

static inline void futex_wake(_Atomic int *address) {
    __builtin_wasm_memory_atomic_notify((int *)address, 1);
}

static inline void initialize_stack_guard(void *stack_pointer) {
    *(uint32_t *)stack_pointer = STACK_GUARD;
}

static inline void check_stack_guard(pthread_t thread) {
    if (thread->stack_pointer &&
        *(uint32_t *)thread->stack_pointer != STACK_GUARD)
        __builtin_trap();
}

typedef struct {
    void *stack_pointer;
    void *tls_base;
    void *(*routine)(void *);
    void *argument;
} context_t;

int __pthread_create(pthread_t nullable * nonnull __restrict result,
                     const pthread_attr_t * nullable __restrict attrp,
                     void * nullable (* nonnull routine)(void * nullable),
                     void * nullable __restrict argument) {
    *result = NULL;
    
    int stacksize = attrp ? attrp->stacksize : STACK_SIZE;
    if (stacksize < 16) stacksize = STACK_SIZE;
    stacksize = (stacksize + 15) & ~15; /* ensure 16‑byte alignment */
    
    /* Allocate stack */
    void *stack_pointer = aligned_alloc(16, stacksize);
    if (!stack_pointer) return -1;
    initialize_stack_guard(stack_pointer);
    
    /* Allocate and initialise TLS */
    size_t tls_size  = __builtin_wasm_tls_size();
    size_t tls_align = __builtin_wasm_tls_align();
    void *tls_base = aligned_alloc(tls_align, tls_size);
    if (!tls_base) {
        free(stack_pointer);
        return -1;
    }
    memset(tls_base, 0, tls_size);
    
    /* Compute the address of the new thread's self struct inside its TLS */
    ptrdiff_t self_offset = (char *)&__pthread_self_internal -
        (char *)__builtin_wasm_tls_base();
    struct opaque_pthread_t *thread =
    (struct opaque_pthread_t *)((char *)tls_base + self_offset);
    
    /* Initialise the thread descriptor */
    thread->tid = 0; /* filled by __wasi_thread_start_C */
    thread->result = NULL;
    int detach = attrp ? attrp->detachstate : PTHREAD_CREATE_JOINABLE;
    atomic_store(&thread->state, detach == PTHREAD_CREATE_DETACHED ?
                 THREAD_DETACHED : THREAD_JOINABLE);
    thread->stack_pointer = stack_pointer;
    thread->tls_base = tls_base;
    
    /* Place the context structure at the TOP of the stack */
    uintptr_t stack_top = (uintptr_t)stack_pointer + stacksize;
    uintptr_t sp = (stack_top - sizeof(context_t)) & ~(uintptr_t)15;
    context_t *context = (context_t *)sp;
    
    context->stack_pointer = (void *)sp; /* initial SP for the new thread */
    context->tls_base      = tls_base;
    context->routine       = routine;
    context->argument      = argument;
    
    int tid = __wasi_thread_spawn(context);
    
    if (tid < 0) {
        free(tls_base);
        free(stack_pointer);
        return -1;
    }
    *result = (pthread_t)thread;
    return 0;
}

int __pthread_detach(pthread_t nonnull thread) {
    int state = THREAD_JOINABLE;
    if (atomic_compare_exchange_strong(&thread->state, &state, THREAD_DETACHED))
        return 0;
    
    if (state == THREAD_EXITED) {
        void *stack_pointer = thread->stack_pointer;
        void *tls_base = thread->tls_base;
        thread->stack_pointer = NULL;
        thread->tls_base = NULL;
        free(stack_pointer);
        free(tls_base);
        return 0;
    }
    return -1;
}

int __pthread_equal(pthread_t nullable lhs, pthread_t nullable rhs) {
    return lhs == rhs;
}

void __pthread_exit(void* nullable result) {
    pthread_t self = __pthread_self();
    check_stack_guard(self);
    self->result = result;
    
    int state = atomic_exchange(&self->state, THREAD_EXITED);
    if (state == THREAD_JOINABLE) {
        futex_wake(&self->state);
        return;
    }
    
    if (state == THREAD_DETACHED) {
        void *stack_pointer = self->stack_pointer;
        void *tls_base = self->tls_base;
        self->stack_pointer = NULL;
        self->tls_base = NULL;
        free(stack_pointer);
        free(tls_base);
        return;
    }
    
    __builtin_trap();
}

int __pthread_join(pthread_t nonnull thread, void* nullable * nullable result) {
    int state;
    while ((state = atomic_load(&thread->state)) != THREAD_EXITED) {
        if (state == THREAD_DETACHED) return -1;
        futex_wait(&thread->state, state);
    }
    
    if (result) *result = thread->result;
    
    void *stack_pointer = thread->stack_pointer;
    void *tls_base = thread->tls_base;
    thread->tls_base = NULL;
    thread->stack_pointer = NULL;
    free(stack_pointer);
    free(tls_base);
    return 0;
}

weak_alias(__pthread_join, pthread_join);
weak_alias(__pthread_detach, pthread_detach);
weak_alias(__pthread_exit, pthread_exit);
weak_alias(__pthread_create, pthread_create);
weak_alias(__pthread_equal, pthread_equal);
weak_alias(__pthread_self, pthread_self);

void __wasi_thread_start(int tid, void *context);
hidden void *__wasi_thread_start_ref = (void *)__wasi_thread_start;

hidden void __wasi_thread_start_C(int tid, void *context) {
    context_t *entry = context;
    
    __pthread_self()->tid = tid;
    __pthread_exit(entry->routine(entry->argument));
}

#endif
