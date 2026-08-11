#if defined(__wasm__)

#include <malloc.h>

extern void __wasm_init_tls(void*);

void *tls = NULL;

__attribute__((constructor))
void __init_tls(void) {
    if (tls) free(tls);

    size_t tls_size  = __builtin_wasm_tls_size();
    size_t tls_align = __builtin_wasm_tls_align();
    tls = aligned_alloc(tls_align, tls_size);
    if (!tls) __builtin_trap();
    
    __wasm_init_tls(tls);
}

__attribute__((destructor))
void __deinit_tls(void) {
    free(tls);
    tls = NULL;
}

#endif
