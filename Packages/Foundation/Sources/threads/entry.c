#if defined(__wasm__)

#include <pthread.h>
#include <stddef.h>

typedef void (*exit_function)(void *);

typedef struct {
    exit_function function;
    void *argument;
    void *dso;
} exit_entry;

#define EXIT_FUNCTION_CAPACITY 32

static exit_entry exit_entries[EXIT_FUNCTION_CAPACITY];
static size_t exit_entry_count = 0;

extern int __main_argc_argv(int argc, char **argv);
extern void __wasm_call_ctors(void);

int __cxa_atexit(exit_function function, void *argument, void *dso) {
    if (exit_entry_count == EXIT_FUNCTION_CAPACITY)
        return -1;
    exit_entries[exit_entry_count++] = (exit_entry){ function, argument, dso };
    return 0;
}

void __cxa_finalize(void *dso) {
    for (;;) {
        size_t index = exit_entry_count;
        while (index && (!exit_entries[index - 1].function ||
                         (dso && exit_entries[index - 1].dso != dso)))
            --index;
        if (!index) break;

        exit_entry entry = exit_entries[index - 1];
        exit_entries[index - 1] = (exit_entry){ 0 };
        while (exit_entry_count && !exit_entries[exit_entry_count - 1].function)
            --exit_entry_count;
        entry.function(entry.argument);
    }
}

void __wasm_call_dtors(void) {
    __cxa_finalize(NULL);
}

wasm_export("_start")
void _start(void) {
    __wasm_call_ctors();
    pthread_self()->tid = 0;

    char *argument = NULL;
    (void)__main_argc_argv(0, &argument);

    __wasm_call_dtors();
}

never void exit(int code) {
    (void)code;
    __wasm_call_dtors();
    __builtin_trap();
}

#endif
