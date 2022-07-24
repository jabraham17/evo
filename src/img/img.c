
#include "img.h"
#include "common/common.h"
#include <stdlib.h>

// PRIVATE
static void
set_pixel(img_pixel_t* pixel, uint8_t blue, uint8_t green, uint8_t red) {
    pixel->blue = blue;
    pixel->green = green;
    pixel->red = red;
}

// PUBLIC
img_t* img_create(size_t width, size_t height) {
    img_t* img = malloc(sizeof(*img));
    size_t size = width * height;
    img->pixels = calloc(size, sizeof(*img->pixels)); // init to zero (NULL)
    img->width = width;
    img->height = height;
    return img;
}

size_t img_get_size(img_t* img) { return img->width * img->height; }
img_pixel_t* img_get_pixel(img_t* img, size_t col, size_t row) {
    size_t idx = common_get_idx(col, row, img->width);
    return &img->pixels[idx];
}
void img_set_bgr(
    img_t* img,
    uint8_t blue,
    uint8_t green,
    uint8_t red,
    size_t col,
    size_t row) {
    img_pixel_t* pixel = img_get_pixel(img, col, row);
    set_pixel(pixel, blue, green, red);
}

void img_destroy(img_t* img) {
    free(img->pixels);
    free(img);
}
