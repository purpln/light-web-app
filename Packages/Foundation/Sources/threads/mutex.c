#if defined(__wasm__)

#include <pthread.h>
#include <stdatomic.h>

static inline void futex_wait(_Atomic int *address, int expected) {
    __builtin_wasm_memory_atomic_wait32((int *)address, expected, -1);
}

static inline void futex_wake(_Atomic int *address) {
    __builtin_wasm_memory_atomic_notify((int *)address, 1);
}

int __pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    *attr = PTHREAD_MUTEX_DEFAULT;
    return 0;
}

int __pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    (void)attr;
    return 0;
}

int __pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    *attr = (long)type;
    return 0;
}

int __pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
    *type = (int)*attr;
    return 0;
}

int __pthread_mutex_init(pthread_mutex_t *mutex,
                         const pthread_mutexattr_t *attr) {
    atomic_init(&mutex->lock, 0);
    mutex->type = attr ? (int)*attr : PTHREAD_MUTEX_DEFAULT;
    return 0;
}

int __pthread_mutex_lock(pthread_mutex_t *mutex) {
    int c = 0;
    if (atomic_compare_exchange_strong(&mutex->lock, &c, 1))
        return 0;
    if (c != 2)
        c = atomic_exchange(&mutex->lock, 2);
    while (c != 0) {
        futex_wait(&mutex->lock, 2);
        c = atomic_exchange(&mutex->lock, 2);
    }
    return 0;
}

int __pthread_mutex_unlock(pthread_mutex_t *mutex) {
    if (atomic_exchange(&mutex->lock, 0) == 2)
        futex_wake(&mutex->lock);
    return 0;
}

int __pthread_mutex_destroy(pthread_mutex_t *mutex) {
    (void)mutex;
    return 0;
}

weak_alias(__pthread_mutex_init,       pthread_mutex_init);
weak_alias(__pthread_mutex_lock,       pthread_mutex_lock);
weak_alias(__pthread_mutex_unlock,     pthread_mutex_unlock);
weak_alias(__pthread_mutex_destroy,    pthread_mutex_destroy);

weak_alias(__pthread_mutexattr_init,    pthread_mutexattr_init);
weak_alias(__pthread_mutexattr_destroy, pthread_mutexattr_destroy);
weak_alias(__pthread_mutexattr_settype, pthread_mutexattr_settype);
weak_alias(__pthread_mutexattr_gettype, pthread_mutexattr_gettype);

#endif
