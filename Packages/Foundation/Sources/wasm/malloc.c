#if defined(__wasm__)

#include <malloc.h>
#include <stdatomic.h>
#include <string.h>

#define PAGE_SIZE 0x10000

extern char __heap_base[];
extern char __heap_end[];

typedef struct Region Region;
struct Region {
    size_t size;
    Region *next;
    uint64_t padding;
    char data[];
};

static_assert(offsetof(Region, data) == 16,
              "malloc payload must be 16-byte aligned");

static Region *heap_end   = NULL;
static Region *next       = (Region *)__heap_base;
static Region *free_list  = NULL;

_Atomic int heap_lock = 0;

static inline void lock_heap(void) {
    int expected = 0;
    while (!atomic_compare_exchange_strong_explicit(
               &heap_lock, &expected, 1,
               memory_order_acquire, memory_order_relaxed)) {
        if (expected == 1) {
            __builtin_wasm_memory_atomic_wait32((int *)&heap_lock, 1, -1);
        }
        expected = 0;
    }
}

static inline void unlock_heap(void) {
    atomic_store_explicit(&heap_lock, 0, memory_order_release);
    __builtin_wasm_memory_atomic_notify((int *)&heap_lock, 1);
}

static inline void *get_heap_end(void) {
    return (void *)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
}

static inline int grow_heap(size_t size) {
    size_t new_page_count = size / PAGE_SIZE + (size % PAGE_SIZE != 0);
    return __builtin_wasm_memory_grow(0, new_page_count) != SIZE_MAX;
}

static inline Region *region_for_ptr(void *ptr) {
    return ((Region *)ptr) - 1;
}

static inline Region *region_after(Region *self, size_t len) {
    uintptr_t address = (uintptr_t)self->data;
    if (len > SIZE_MAX - address || address + len > SIZE_MAX - 15)
        return NULL;
    return (Region *)__builtin_align_up(address + len, 16);
}

static inline int grow_heap_for_region(Region *region_end) {
    if (!region_end) return 0;
    
    uintptr_t required = (uintptr_t)region_end;
    uintptr_t current = (uintptr_t)heap_end;
    if (required > current) {
        if (!grow_heap(required - current))
            return 0;
        heap_end = get_heap_end();
    }
    return required <= (uintptr_t)heap_end;
}

static void *malloc_locked(size_t size) {
    if (size == 0) return NULL;
    
    if (heap_end == NULL)
        heap_end = get_heap_end();
    
    Region *prev = NULL;
    Region *curr = free_list;
    while (curr != NULL) {
        if (curr->size >= size) {
            if (prev == NULL)
                free_list = curr->next;
            else
                prev->next = curr->next;
            return &curr->data;
        }
        prev = curr;
        curr = curr->next;
    }
    
    Region *region_end = region_after(next, size);
    if (!grow_heap_for_region(region_end))
        return NULL;
    
    void *result = &next->data;
    next->size = size;
    next = region_end;
    return result;
}

static void free_locked(void *ptr) {
    if (ptr == NULL) return;
    
    Region *region = region_for_ptr(ptr);
    Region *region_end = region_after(region, region->size);
    
    if (region_end == next) {
        next = region;
    } else {
        region->next = free_list;
        free_list = region;
    }
}

static void *aligned_alloc_locked(size_t alignment, size_t size) {
    if (size == 0) return NULL;
    if (alignment == 0) alignment = 1;
    if (alignment & (alignment - 1)) return NULL;
    
    Region *prev = NULL;
    Region *curr = free_list;
    while (curr != NULL) {
        if (((uintptr_t)curr->data & (alignment - 1)) == 0 &&
            curr->size >= size) {
            if (prev == NULL)
                free_list = curr->next;
            else
                prev->next = curr->next;
            return &curr->data;
        }
        prev = curr;
        curr = curr->next;
    }
    
    if (heap_end == NULL)
        heap_end = get_heap_end();
    
    size_t offset = offsetof(Region, data);
    uintptr_t curr_data = (uintptr_t)next->data;
    if (curr_data > SIZE_MAX - (alignment - 1))
        return NULL;
    uintptr_t aligned_data = __builtin_align_up(curr_data, alignment);
    Region *aligned_region = (Region *)(aligned_data - offset);
    Region *alloc_end = region_after(aligned_region, size);
    
    if (!grow_heap_for_region(alloc_end))
        return NULL;
    
    aligned_region->size = size;
    next = alloc_end;
    return &aligned_region->data;
}

wasm_export("malloc")
noinline void *__malloc(size_t size) {
    lock_heap();
    void *result = malloc_locked(size);
    unlock_heap();
    return result;
}

wasm_export("free")
noinline void __free(void *pointer) {
    lock_heap();
    free_locked(pointer);
    unlock_heap();
}

noinline void *__calloc(size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size)
        return NULL;
    size_t total = count * size;
    
    lock_heap();
    void *result = malloc_locked(total);
    if (result)
        memset(result, 0, total);
    unlock_heap();
    return result;
}

noinline void *__realloc(void *pointer, size_t new_size) {
    lock_heap();
    
    if (pointer == NULL) {
        void *result = malloc_locked(new_size);
        unlock_heap();
        return result;
    }
    
    if (new_size == 0) {
        free_locked(pointer);
        unlock_heap();
        return NULL;
    }
    
    Region *region = region_for_ptr(pointer);
    Region *region_end = region_after(region, region->size);
    
    if (region_end == next) {
        Region *new_region_end = region_after(region, new_size);
        if (grow_heap_for_region(new_region_end)) {
            region->size = new_size;
            next = new_region_end;
            unlock_heap();
            return &region->data;
        }
    }
    
    void *result = malloc_locked(new_size);
    if (result) {
        size_t copy_size = region->size < new_size ? region->size : new_size;
        memcpy(result, &region->data, copy_size);
        free_locked(pointer);
    }
    unlock_heap();
    return result;
}

always_inline never void __abort(void) {
    __builtin_trap();
}

noinline void *__aligned_alloc(size_t alignment, size_t size) {
    lock_heap();
    void *pointer = aligned_alloc_locked(alignment, size);
    unlock_heap();
    return pointer;
}

hidden int __posix_memalign(void **memptr, size_t alignment, size_t size) {
    if (alignment < sizeof(void*) || (alignment & (alignment - 1)) != 0)
        return -1;
    void *ptr = __aligned_alloc(alignment, size);
    if (ptr == NULL) return -1;
    *memptr = ptr;
    return 0;
}

weak_alias(__malloc, malloc);
weak_alias(__free, free);
weak_alias(__calloc, calloc);
weak_alias(__realloc, realloc);
weak_alias(__abort, abort);
weak_alias(__aligned_alloc, aligned_alloc);
weak_alias(__posix_memalign, posix_memalign);

#endif
