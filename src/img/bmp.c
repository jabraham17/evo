
#include "bmp.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private

// every row must be multiple of 4
static size_t bmp_row_padding_length(size_t width, size_t bits_per_pixel) {
    size_t bytes_per_pixel = bits_per_pixel / 8;
    size_t row_multiple = 4;

    size_t bytes_per_row = width * bytes_per_pixel;

    // a good compiler will remove these mods :)
    return (row_multiple - bytes_per_row % 4) % 4;
}

static size_t
bmp_get_data_idx(size_t col, size_t row, size_t width, size_t bits_per_pixel) {
    size_t bytes_per_pixel = bits_per_pixel / 8;
    return (row * width * bytes_per_pixel) +
           (row * bmp_row_padding_length(width, bits_per_pixel)) +
           (col * bytes_per_pixel);
}

static size_t
bmp_data_size(size_t width, size_t height, size_t bits_per_pixel) {
    size_t bytes_per_pixel = bits_per_pixel / 8;
    size_t bytes_per_row =
        width * bytes_per_pixel + bmp_row_padding_length(width, bits_per_pixel);
    return bytes_per_row * height;
}

static size_t bmp_file_size(
    size_t header_size,
    size_t dib_size,
    size_t width,
    size_t height,
    size_t bits_per_pixel) {
    return header_size + dib_size +
           bmp_data_size(width, height, bits_per_pixel);
}

static void bmp_create_header(bmp_header_t* header, bmp_dib_t* dib) {

    // file size is header size
    size_t file_size = 0;
    size_t start_address = 0;
    if(dib->type == BMP_DIB_BITMAPINFOHEADER) {
        file_size = bmp_file_size(
            sizeof(*header),
            dib->dib_struct.BITMAPINFOHEADER.header_size,
            dib->dib_struct.BITMAPINFOHEADER.pixel_width,
            dib->dib_struct.BITMAPINFOHEADER.pixel_height,
            dib->dib_struct.BITMAPINFOHEADER.bits_per_pixel);
        start_address =
            sizeof(*header) + dib->dib_struct.BITMAPINFOHEADER.header_size;
    }

    header->signature = 'M' << 8 | 'B';
    header->size = file_size;
    header->reserved_1 = 0;
    header->reserved_2 = 0;
    header->start_address = start_address;
}

static void bmp_create_dib(bmp_dib_t* dib, size_t width, size_t height) {
    if(dib->type == BMP_DIB_BITMAPINFOHEADER) {
        dib->dib_struct.BITMAPINFOHEADER.header_size = 40;
        dib->dib_struct.BITMAPINFOHEADER.pixel_width = width;
        dib->dib_struct.BITMAPINFOHEADER.pixel_height = height;
        dib->dib_struct.BITMAPINFOHEADER.n_color_planes = 1;
        dib->dib_struct.BITMAPINFOHEADER.bits_per_pixel = 24;
        memset(dib->dib_struct.BITMAPINFOHEADER.reserved, 0, 24);
    }
}

static bmp_t* bmp_create(size_t width, size_t height) {
    bmp_t* bmp = malloc(sizeof(*bmp));

    bmp->dib.type = BMP_DIB_BITMAPINFOHEADER;
    bmp_create_dib(&bmp->dib, width, height);
    bmp_create_header(&bmp->header, &bmp->dib);

    size_t bits_per_pixel = bmp->dib.dib_struct.BITMAPINFOHEADER.bits_per_pixel;
    bmp->data_length = bmp_data_size(width, height, bits_per_pixel);
    bmp->data = calloc(bmp->data_length, sizeof(*bmp->data));

    return bmp;
}

// public

bmp_t* bmp_create_from_img(img_t* img) {
    size_t width = img->width;
    size_t height = img->height;
    bmp_t* bmp = bmp_create(width, height);

    size_t bits_per_pixel = 0;
    if(bmp->dib.type == BMP_DIB_BITMAPINFOHEADER) {
        bits_per_pixel = bmp->dib.dib_struct.BITMAPINFOHEADER.bits_per_pixel;
    }

    // need to iterate in reverse rows
    // pixels[0][0] is the bottom left
    // pixels[0][width-1] is the bottom right
    // pixels[height-1][0] is the top left
    for(size_t i = 0; i < height; i++) {
        size_t img_row = height - 1 - i;
        size_t bmp_row = i;
        for(size_t col = 0; col < width; col++) {
            size_t img_idx = common_get_idx(col, img_row, width);
            img_pixel_t* pixel = &img->pixels[img_idx];
            if(pixel) {
                size_t bmp_idx =
                    bmp_get_data_idx(col, bmp_row, width, bits_per_pixel);
                bmp->data[bmp_idx] = pixel->blue;
                bmp->data[bmp_idx + 1] = pixel->green;
                bmp->data[bmp_idx + 2] = pixel->red;
            }
        }
    }
    return bmp;
}

// TODO: rwrite to write to a block of memory and reurn the block of memory
// then a separate func copies the block of memory to a file
void bmp_write_to_file(bmp_t* bmp, char* filename) {
    void* dib_struct = NULL;
    size_t dib_len = 0;
    if(bmp->dib.type == BMP_DIB_BITMAPINFOHEADER) {
        dib_struct = &bmp->dib.dib_struct.BITMAPINFOHEADER;
        dib_len = sizeof(bmp->dib.dib_struct.BITMAPINFOHEADER);
    } else return;

    FILE* fp = fopen(filename, "wb");

    fwrite(&bmp->header, sizeof(bmp->header), 1, fp);
    fwrite(dib_struct, dib_len, 1, fp);

    fwrite(bmp->data, bmp->data_length, 1, fp);
    fclose(fp);
}

void bmp_destroy(bmp_t* bmp) {
    free(bmp->data);
    free(bmp);
}
