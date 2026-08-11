#if defined(__wasm__)

#include <stdint.h>

uintptr_t __stack_chk_guard = 0x9e3779b9u;

void __stack_chk_guard_setup(void) {}

__attribute__((noreturn))
void __stack_chk_fail(void) {
    __builtin_trap();
}

#endif
