#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

typedef struct {
  uint8_t b, g, r;
} pixel_t;

typedef struct {
  uint64_t width, height;
  pixel_t* data;
} image_t;

image_t image_rotate_copy_180(const image_t image);

image_t image_rotate_copy_90cw(const image_t image);

#endif
