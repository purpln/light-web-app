#pragma once

#include <stddef.h>

size_t strlen(const char *string);
int memcmp(const void *lhs, const void *rhs, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
