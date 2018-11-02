#include <stdio.h>
#include <stdlib.h>
#include "image_bmp.h"

bmp_read_status_t image_read_bmp(FILE* source_bmp, image_t* image, bmp_header_t* header) {
  fread(header, 1, sizeof(bmp_header_t), source_bmp);

  if (header->magic[0] != 'B' || header->magic[1] != 'M')
    return READ_INVALID_SIGNATURE;

  if (header->bits_per_pixel != 24)
    return READ_INVALID_BITS;

  uint8_t* raw_data = (uint8_t*) malloc(header->data_size);

  fseek(source_bmp, header->data_offset, SEEK_SET);
  fread(raw_data, header->data_size, 1, source_bmp);

  if (header->width % 4 == 0)
    image->data = (pixel_t*) raw_data;
  else {
    pixel_t* unpadded = (pixel_t*) malloc(header->width * header->height * sizeof(pixel_t));

    for (uint64_t h = 0; h < header->height; h++) {
      /* Padding accumulated since the first row = num of rows * padding per row */
      uint64_t padding = h * (header->width % 4);
      for (uint64_t w = 0; w < header->width; w++) {
        uint64_t pixel_i = h * header->width + w;
        unpadded[pixel_i] = *(pixel_t*) (raw_data + sizeof(pixel_t) * pixel_i + padding);
      }
    }

    free(raw_data);
    image->data = unpadded;
  }

  image->width = header->width;
  image->height = header->height;

  return READ_OK;
}

bmp_write_status_t image_write_bmp(FILE* dest_bmp, const image_t image, const bmp_header_t header) {
  fwrite(&header, sizeof(bmp_header_t), 1, dest_bmp);

  if (sizeof(bmp_header_t) < header.data_offset) {
    uint32_t offset = header.data_offset - sizeof(bmp_header_t);
    char* padding = calloc(offset, sizeof(char));
    fwrite(padding, sizeof(char), offset, dest_bmp);
  }

  if (header.width % 4 == 0)
    fwrite(image.data, 1, header.width * header.height * sizeof(pixel_t), dest_bmp);
  else {
    uint64_t data_size = header.width * header.height * sizeof(pixel_t) + header.height * (header.width % 4);
    uint8_t* data = (uint8_t*) calloc(1, data_size);
    for (uint64_t h = 0; h < header.height; h++) {
      uint64_t padding = h * (header.width % 4);
      for (uint64_t w = 0; w < header.width; w++) {
        uint64_t pixel_i = h * header.width + w;
        *((pixel_t*) (data + sizeof(pixel_t) * pixel_i + padding)) = image.data[pixel_i];
      }
    }
    fwrite(data, 1, data_size, dest_bmp);
    free(data);
  }

  fflush(dest_bmp);

  return WRITE_OK;
}
