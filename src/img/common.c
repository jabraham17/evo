#include "common.h"

size_t common_get_idx(size_t col, size_t row, size_t width) {
    return row * width + col;
}
size_t common_get_col(size_t idx, size_t width) {
    return idx%width;
}
size_t common_get_row(size_t idx, size_t width) {
    return idx/width;
}
