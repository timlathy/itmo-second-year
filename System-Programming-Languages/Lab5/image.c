#include <stdlib.h>
#include "image.h"

image_t image_rotate_copy_180(const image_t image) {
  pixel_t* data_rot = (pixel_t*) malloc(image.width * image.height * sizeof(pixel_t));

  for (uint64_t h = 0; h < image.height; h++)
    for (uint64_t w = 0; w < image.width; w++)
      data_rot[(h * image.width) + w] =
        image.data[((image.height - 1 - h) * image.width) + w];

  return (image_t) { .width = image.width, .height = image.height, .data = data_rot };
}

image_t image_rotate_copy_90cw(const image_t image) {
  pixel_t* data_rot = (pixel_t*) malloc(image.width * image.height * sizeof(pixel_t));

  for (uint64_t h = 0; h < image.height; h++)
    for (uint64_t w = 0; w < image.width; w++)
      data_rot[(w * image.height) + h] =
        image.data[(h * image.width) + w];

  return (image_t) { .width = image.height, .height = image.width, .data = data_rot };
}
