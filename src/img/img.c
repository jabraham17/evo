
#include "img.h"
#include "common/common.h"
#include <stdlib.h>

// PRIVATE
static img_pixel_t* make_pixel(uint8_t blue, uint8_t green, uint8_t red) {
    img_pixel_t* pixel = malloc(sizeof(*pixel));
    pixel->blue = blue;
    pixel->green = green;
    pixel->red = red;
    return pixel;
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
void img_set_pixel(img_t* img, img_pixel_t* pixel, size_t col, size_t row) {
    size_t idx = common_get_idx(col, row, img->width);
    img->pixels[idx] = pixel;
}
img_pixel_t* img_get_pixel(img_t* img, size_t col, size_t row) {
    size_t idx = common_get_idx(col, row, img->width);
    return img->pixels[idx];
}
void img_set_bgr(
    img_t* img,
    uint8_t blue,
    uint8_t green,
    uint8_t red,
    size_t col,
    size_t row) {
    img_pixel_t* pixel = make_pixel(blue, green, red);
    img_set_pixel(img, pixel, col, row);
}

void img_destroy(img_t* img) {
    size_t size = img_get_size(img);
    for(size_t i = 0; i < size; i++) {
        if(img->pixels[i]) free(img->pixels[i]);
    }
    free(img->pixels);
    free(img);
}
