#if defined(__wasm__)

#include <stdint.h>

typedef __int128_t int128_t;

typedef union {
    int128_t value;
    struct {
        uint64_t low;
        uint64_t high;
    } words;
} int128_words;

static uint64_t multiply_high(uint64_t lhs, uint64_t rhs) {
    uint64_t lhs_low = (uint32_t)lhs;
    uint64_t lhs_high = lhs >> 32;
    uint64_t rhs_low = (uint32_t)rhs;
    uint64_t rhs_high = rhs >> 32;
    uint64_t low = lhs_low * rhs_low;
    uint64_t middle = lhs_high * rhs_low + (low >> 32);
    uint64_t carry = middle >> 32;
    middle = (uint32_t)middle + lhs_low * rhs_high;
    return lhs_high * rhs_high + carry + (middle >> 32);
}

int128_t __multi3(int128_t a, int128_t b) {
    int128_words lhs = { .value = a };
    int128_words rhs = { .value = b };
    int128_words result;
    result.words.low = lhs.words.low * rhs.words.low;
    result.words.high = multiply_high(lhs.words.low, rhs.words.low)
        + lhs.words.high * rhs.words.low
        + lhs.words.low * rhs.words.high;
    return result.value;
}

#endif
