#pragma once

#include <stddef.h>
#include <features.h>

void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t new_size);
void* aligned_alloc(size_t alignment, size_t size);

never void abort(void);
