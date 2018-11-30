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

#endif
