#if defined(__wasm__)

#include <pthread.h>

int __pthread_attr_init(pthread_attr_t *attr) {
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    attr->stacksize   = 0;
    return 0;
}

int __pthread_attr_destroy(pthread_attr_t *attr) {
    (void)attr;
    return 0;
}

int __pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    if (detachstate != PTHREAD_CREATE_JOINABLE &&
        detachstate != PTHREAD_CREATE_DETACHED)
        return -1;
    attr->detachstate = detachstate;
    return 0;
}

int __pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
    *detachstate = attr->detachstate;
    return 0;
}

int __pthread_attr_setstacksize(pthread_attr_t *attr, int stacksize) {
    if (stacksize < 16)
        return -1;
    attr->stacksize = stacksize;
    return 0;
}

int __pthread_attr_getstacksize(const pthread_attr_t *attr, int *stacksize) {
    *stacksize = attr->stacksize;
    return 0;
}

weak_alias(__pthread_attr_init,           pthread_attr_init);
weak_alias(__pthread_attr_destroy,        pthread_attr_destroy);
weak_alias(__pthread_attr_setdetachstate, pthread_attr_setdetachstate);
weak_alias(__pthread_attr_getdetachstate, pthread_attr_getdetachstate);
weak_alias(__pthread_attr_setstacksize,   pthread_attr_setstacksize);
weak_alias(__pthread_attr_getstacksize,   pthread_attr_getstacksize);

#endif
