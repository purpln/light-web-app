#pragma once

#if defined(__wasm__)

#include <features.h>

wasm_import("wasi", "thread-spawn") int __wasi_thread_spawn(void *argument);

#endif
