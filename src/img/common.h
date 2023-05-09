#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <stddef.h>
#include <stdint.h>

size_t common_get_idx(size_t col, size_t row, size_t width);
size_t common_get_col(size_t idx, size_t width);
size_t common_get_row(size_t idx, size_t width);

void* common_stack_to_heap(void* stack_ptr, size_t length);

__attribute__((always_inline)) float clampf(float value, float min, float max);
__attribute__((always_inline)) int8_t
clampb(int8_t value, int8_t min, int8_t max);
__attribute__((always_inline)) int16_t
clampw(int16_t value, int16_t min, int16_t max);
__attribute__((always_inline)) int32_t
clampd(int32_t value, int32_t min, int32_t max);
__attribute__((always_inline)) int64_t
clampq(int64_t value, int64_t min, int64_t max);

#endif
