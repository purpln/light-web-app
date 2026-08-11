#if defined(__wasm__)

#include <string.h>

#define BULK_MEMORY_THRESHOLD 16

size_t strlen(const char *string) {
    if (!string) return 0;
    const char *end = string;
    while (*end) end++;
    return (size_t)(end - string);
}

int memcmp(const void *lhs, const void *rhs, size_t n) {
#if defined(__wasm_bulk_memory__)
    if (n > BULK_MEMORY_THRESHOLD) return __builtin_memcmp(lhs, rhs, n);
#endif
    const unsigned char *l = lhs;
    const unsigned char *r = rhs;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
#if defined(__wasm_bulk_memory__)
    if (n > BULK_MEMORY_THRESHOLD) return __builtin_memcpy(dest, src, n);
#endif
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (; n; n--) *d++ = *s++;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
#if defined(__wasm_bulk_memory__)
    if (n > BULK_MEMORY_THRESHOLD) return __builtin_memmove(dest, src, n);
#endif
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        d += n;
        s += n;
        while (n--) {
            *(--d) = *(--s);
        }
    }
    return dest;
}

void *memset(void *dest, int c, size_t n) {
#if defined(__wasm_bulk_memory__)
    if (n > BULK_MEMORY_THRESHOLD) return __builtin_memset(dest, c, n);
#endif
    unsigned char *p = dest;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return dest;
}

#endif
