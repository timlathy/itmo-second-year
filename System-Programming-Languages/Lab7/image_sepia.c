#include <stdio.h>
#include "image_sepia.h"

static unsigned char sat(uint64_t x) {
  return x < 256 ? x : 255;
}

static void sepia_one(pixel_t* pixel) {
  static const float c[3][3] = {
    { .393f, .769f, .189f },
    { .349f, .686f, .168f },
    { .272f, .543f, .131f }
  };
  pixel_t old = *pixel;
  pixel->r = sat(old.r * c[0][0] + old.g * c[0][1] + old.b * c[0][2]);
  pixel->g = sat(old.r * c[1][0] + old.g * c[1][1] + old.b * c[1][2]);
  pixel->b = sat(old.r * c[2][0] + old.g * c[2][1] + old.b * c[2][2]);
}

extern void image_sepia_sse(pixel_t* pixel, uint64_t size);
extern void image_sepia_avx(pixel_t* pixel, uint64_t size);
 
void sepia_sse_inplace(image_t* img) {
  uint64_t num_pixels = img->height * img->width;

  if (num_pixels < 4) { for (int p = 0; p < num_pixels; ++p) sepia_one(img->data + p); return; }

  image_sepia_sse(img->data, num_pixels - num_pixels % 4);

  for (int p = num_pixels - num_pixels % 4; p < num_pixels; ++p) sepia_one(img->data + p);
}

void sepia_avx_inplace(image_t* img) {
  uint64_t num_pixels = img->height * img->width;

  if (num_pixels < 8) { for (int p = 0; p < num_pixels; ++p) sepia_one(img->data + p); return; }

  image_sepia_avx(img->data, num_pixels - num_pixels % 8);

  for (int p = num_pixels - num_pixels % 8; p < num_pixels; ++p) sepia_one(img->data + p);
}

void sepia_naive_inplace(image_t* img) {
  for (uint32_t h = 0; h < img->height; h++)
    for (uint32_t w = 0; w < img->width; w++)
      sepia_one(img->data + h * img->width + w);
}

