#ifndef IMAGE_BMP_H
#define IMAGE_BMP_H

#include "image.h"
#include <stdio.h>

typedef enum {
  READ_OK = 0,
  READ_INVALID_SIGNATURE,
  READ_UNSUPPORTED_BITS_PER_PIXEL,
  READ_ERROR
} bmp_read_status_t;

typedef enum {
  WRITE_OK = 0,
  WRITE_ERROR
} bmp_write_status_t;

typedef struct __attribute__((packed)) {
  char magic[2];
  uint32_t file_size;
  uint32_t reserved0;
  uint32_t data_offset;
} bmp_file_header_t;

typedef struct __attribute__((packed)) {
  uint32_t header_size;
  uint32_t width;
  uint32_t height;
  uint16_t num_colorplanes;
  uint16_t bits_per_pixel;
  uint32_t compression_method;
  uint32_t data_size;
  uint32_t h_pixels_per_meter;
  uint32_t w_pixels_per_meter;
  uint32_t num_colors;
  uint32_t num_important_colors;
} bmp_bitmap_header_t;

typedef struct __attribute__((packed)) {
  bmp_file_header_t file;
  bmp_bitmap_header_t bitmap;
} bmp_header_t;

bmp_read_status_t image_read_bmp(FILE* source_bmp, image_t* image);

bmp_write_status_t image_write_bmp(FILE* dest_bmp, const image_t image);

#endif
