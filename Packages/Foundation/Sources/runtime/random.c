#if defined(__wasm__)

#include <stdatomic.h>
#include <stdint.h>

static _Atomic uint32_t random_state = 0x6d2b79f5u;

void arc4random_buf(void *buffer, size_t size) {
    uint8_t *bytes = buffer;
    while (size) {
        uint32_t value = atomic_fetch_add(&random_state, 0x9e3779b9u);
        value ^= value >> 16;
        value *= 0x21f0aaadu;
        value ^= value >> 15;
        value *= 0x735a2d97u;
        value ^= value >> 15;

        for (size_t index = 0; index < sizeof(value) && size; ++index, --size)
            *bytes++ = (uint8_t)(value >> (index * 8));
    }
}

#endif
