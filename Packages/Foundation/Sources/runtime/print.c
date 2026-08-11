#if defined(__wasm__)

#include <stdint.h>

#define BUFFER_SIZE 1024

extern int puts(char *c);

int putchar(int c) {
    static _Thread_local char letters[BUFFER_SIZE];
    static _Thread_local int length = 0;
    
    // Handle null terminator or buffer full
    if (c == '\n' || c == '\0' || length >= BUFFER_SIZE - 1) {
        // Ensure null termination
        if (length < BUFFER_SIZE) {
            letters[length] = '\0';
        } else {
            letters[BUFFER_SIZE - 1] = '\0';
        }
        
        puts(letters); // puts adds newline automatically
        length = 0;
        
        // If it was a regular character that filled the buffer,
        // we still need to handle the current character
        if (c != '\n' && c != '\0' && length < BUFFER_SIZE - 1) {
            letters[0] = (char)c;
            length = 1;
        }
    } else {
        letters[length] = (char)c;
        length++;
    }
    return c;
}

#endif
