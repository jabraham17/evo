#ifndef IMG_IMG_H_
#define IMG_IMG_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} img_pixel_t;

typedef struct {
    img_pixel_t** pixels;
    size_t width;
    size_t height;
} img_t;

img_t* img_create(size_t width, size_t height);
size_t img_get_size(img_t*);
void img_set_pixel(img_t*, img_pixel_t*, size_t col, size_t row);
img_pixel_t* img_get_pixel(img_t*, size_t col, size_t row);
void img_set_bgr(
    img_t*,
    uint8_t blue,
    uint8_t green,
    uint8_t red,
    size_t col,
    size_t row);
void img_destroy(img_t*);

#endif
