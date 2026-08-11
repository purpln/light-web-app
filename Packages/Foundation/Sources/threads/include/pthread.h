#pragma once

#if defined(__wasm__)

#include <features.h>

typedef struct opaque_pthread_t *pthread_t;

struct opaque_pthread_t {
    int tid;
    void * nullable result;
    _Atomic int state;
    void * nullable stack_pointer;
    void * nullable tls_base;
};

typedef struct {
    _Atomic int lock;
    int type;
} pthread_mutex_t;

typedef long pthread_mutexattr_t;

typedef struct {
    int detachstate;
    int stacksize;
} pthread_attr_t;

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_MUTEX_INITIALIZER {0, PTHREAD_MUTEX_DEFAULT}
#define PTHREAD_MUTEXATTR_INITIALIZER 0L

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE  2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

int pthread_attr_init(pthread_attr_t* nonnull attr);
int pthread_attr_destroy(pthread_attr_t* nonnull attr);
int pthread_attr_setdetachstate(pthread_attr_t* nonnull attr,
                                int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t* nonnull attr,
                                int* nonnull detachstate);
int pthread_attr_setstacksize(pthread_attr_t* nonnull attr, int stacksize);
int pthread_attr_getstacksize(const pthread_attr_t* nonnull attr,
                              int* nonnull stacksize);

nonnull pthread_t pthread_self(void);
int pthread_create(pthread_t nullable * nonnull __restrict,
                   const pthread_attr_t * nullable __restrict,
                   void * nullable (* nonnull)(void * nullable),
                   void * nullable __restrict);
int pthread_detach(pthread_t nonnull);
int pthread_equal(pthread_t nullable, pthread_t nullable);
void pthread_exit(void * nullable) never;
int pthread_join(pthread_t nonnull, void * nullable * nullable);

int pthread_mutexattr_init(pthread_mutexattr_t* nonnull attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t* nonnull attr);
int pthread_mutexattr_settype(pthread_mutexattr_t* nonnull attr, int type);
int pthread_mutexattr_gettype(const pthread_mutexattr_t* nonnull attr,
                              int* nonnull type);

int pthread_mutex_init(pthread_mutex_t* nonnull mutex,
                       const pthread_mutexattr_t* nullable attr);
int pthread_mutex_lock(pthread_mutex_t* nonnull mutex);
int pthread_mutex_unlock(pthread_mutex_t* nonnull mutex);
int pthread_mutex_destroy(pthread_mutex_t* nonnull mutex);

#endif
