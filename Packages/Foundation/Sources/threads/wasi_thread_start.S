#if defined(__wasm__)
    .text
    
    .export_name __wasi_thread_start, __wasi_thread_start
    
    .globaltype __stack_pointer, i32
    .globaltype __tls_base, i32
    .functype   __wasi_thread_start_C (i32, i32) -> ()
    
    .hidden __wasi_thread_start
    .globl  __wasi_thread_start
    .type   __wasi_thread_start,@function
    
__wasi_thread_start:
    .functype __wasi_thread_start (i32, i32) -> ()
    
    # Set up the minimum C environment.
    local.get 1
    i32.load  0
    global.set __stack_pointer
    
    local.get 1
    i32.load  4
    global.set __tls_base
    
    # Make the C function do the rest of work.
    local.get 0
    local.get 1
    call __wasi_thread_start_C
    
    end_function
#endif
