#include "common.h"
#include <math.h>
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

#define CLAMP_FUNC(value, min, max)                                            \
    do {                                                                       \
        if(value < min) return min;                                            \
        if(value > max) return max;                                            \
        return value;                                                          \
    } while(0)

__attribute__((always_inline)) float clampf(float value, float min, float max) {
    return fmax(min, fmin(max, value));
}
__attribute__((always_inline)) int8_t
clampb(int8_t value, int8_t min, int8_t max) {
    CLAMP_FUNC(value, min, max);
}
__attribute__((always_inline)) int16_t
clampw(int16_t value, int16_t min, int16_t max) {
    CLAMP_FUNC(value, min, max);
}
__attribute__((always_inline)) int32_t
clampd(int32_t value, int32_t min, int32_t max) {
    CLAMP_FUNC(value, min, max);
}
__attribute__((always_inline)) int64_t
clampq(int64_t value, int64_t min, int64_t max) {
    CLAMP_FUNC(value, min, max);
}

#undef CLAMP_FUNC

#define SCALE_FUNC(typein, typeout)                                            \
    __attribute__((always_inline)) TYPE_MAP(typeout)                           \
        scale_##typein##2##typeout(                                            \
            TYPE_MAP(typein) value,                                            \
            TYPE_MAP(typein) oldMin,                                           \
            TYPE_MAP(typein) oldMax,                                           \
            TYPE_MAP(typeout) newMin,                                          \
            TYPE_MAP(typeout) newMax) {                                        \
        TYPE_MAP(typeout)                                                      \
        result = (newMax - newMin) *                                           \
                     ((TYPE_MAP(typeout))value - (TYPE_MAP(typeout))oldMin) /  \
                     ((TYPE_MAP(typeout))oldMax - (TYPE_MAP(typeout))oldMin) + \
                 newMin;                                                       \
        return result;                                                         \
    }
#include "scale_func.def"
#undef SCALE_FUNC
