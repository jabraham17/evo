#ifndef IMG_COMMON_H_
#define IMG_COMMON_H_

#include <stddef.h>


size_t common_get_idx(size_t col, size_t row, size_t width);
size_t common_get_col(size_t idx, size_t width);
size_t common_get_row(size_t idx, size_t width);

#endif
