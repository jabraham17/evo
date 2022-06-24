#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <stddef.h>

size_t common_get_idx(size_t col, size_t row, size_t width);
size_t common_get_col(size_t idx, size_t width);
size_t common_get_row(size_t idx, size_t width);

void* common_stack_to_heap(void* stack_ptr, size_t length);

#endif
