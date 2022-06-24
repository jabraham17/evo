#include "common.h"
#include <stdlib.h>
#include <string.h>

size_t common_get_idx(size_t col, size_t row, size_t width) {
    return row * width + col;
}
size_t common_get_col(size_t idx, size_t width) { return idx % width; }
size_t common_get_row(size_t idx, size_t width) { return idx / width; }

void* common_stack_to_heap(void* stack_ptr, size_t length) {
    void* heap_ptr = malloc(length);
    memcpy(heap_ptr, stack_ptr, length);
    return heap_ptr;
}
