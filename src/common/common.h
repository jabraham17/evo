#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <stddef.h>
#include <stdint.h>

size_t common_get_idx(size_t col, size_t row, size_t width);
size_t common_get_col(size_t idx, size_t width);
size_t common_get_row(size_t idx, size_t width);

void* common_stack_to_heap(void* stack_ptr, size_t length);

float clampf(float value, float min, float max);
int8_t clampb(int8_t value, int8_t min, int8_t max);
int16_t clampw(int16_t value, int16_t min, int16_t max);

#define SCALE_FUNC(typein, typeout)                                            \
    TYPE_MAP(typeout)                                                          \
    scale_##typein##2##typeout(                                                \
        TYPE_MAP(typein) value,                                                \
        TYPE_MAP(typein) oldMin,                                               \
        TYPE_MAP(typein) oldMax,                                               \
        TYPE_MAP(typeout) newMin,                                              \
        TYPE_MAP(typeout) newMax)
#include "scale_func.def"
#undef SCALE_FUNC

#endif
