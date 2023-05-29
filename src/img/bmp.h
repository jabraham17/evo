#ifndef IMG_BMP_H_
#define IMG_BMP_H_

#include "img.h"
#include <stddef.h>
#include <stdint.h>

#define BMP_PACKED __attribute__((packed))

typedef struct {
    uint16_t signature;
    uint32_t size;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint32_t start_address;
} BMP_PACKED bmp_header_t;

typedef enum { BMP_DIB_BITMAPINFOHEADER } bmp_dib_type_t;

typedef struct {
    uint32_t header_size; // 40
    uint32_t pixel_width;
    uint32_t pixel_height;
    uint16_t n_color_planes; // 1
    uint16_t bits_per_pixel;
    uint8_t reserved[24];
} BMP_PACKED bmp_dib_BITMAPINFOHEADER_t;

typedef struct {
    union {
        bmp_dib_BITMAPINFOHEADER_t BITMAPINFOHEADER;
    } dib_struct;
    bmp_dib_type_t type;
} BMP_PACKED bmp_dib_t;

typedef struct {
    bmp_header_t header;
    bmp_dib_t dib;
    size_t data_length;
    uint8_t* data;
} BMP_PACKED bmp_t;

// typedef struct {
//     bmp_header_t header;
//     bmp_dib_t dib;
//     uint_8_t* data;
//     size_t n_data;
// } BMP_PACKED bmp_printable_t;

bmp_t* bmp_create_from_img(img_t*);
void bmp_write_to_file(bmp_t*, const char* filename);
void bmp_destroy(bmp_t*);

#endif
