#pragma once

#define weak __attribute__((__weak__))
#define hidden __attribute__((__visibility__("hidden")))
#define never __attribute__((noreturn))
#define always_inline __attribute__((always_inline))
#define noinline __attribute__((noinline))

#define weak_alias(old, new) \
    extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

#define wasm_import(m, n) \
    __attribute__((import_module(m))) __attribute__((import_name(n)))
#define wasm_export(n) __attribute__((export_name(n)))

#define nullable _Nullable
#define nonnull _Nonnull
#define null_unspecified _Null_unspecified

#define static_assert _Static_assert
