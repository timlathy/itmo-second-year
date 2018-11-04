#include <stdio.h>
#include <stdlib.h>
#include "image_bmp.h"

bmp_header_t image_create_bmp_header(const image_t image) {
  bmp_header_t header;

  header.bitmap.header_size = sizeof(bmp_bitmap_header_t);
  header.bitmap.width = image.width;
  header.bitmap.height = image.height;
  header.bitmap.num_colorplanes = 1;
  header.bitmap.bits_per_pixel = 24;
  header.bitmap.compression_method = 0;
  header.bitmap.h_pixels_per_meter = 2835;
  header.bitmap.w_pixels_per_meter = 2835;
  header.bitmap.num_colors = 0;
  header.bitmap.num_important_colors = 0;

  header.bitmap.data_size = image.width * image.height * sizeof(pixel_t)
    + image.height * (image.width % 4);

  header.file.magic[0] = 'B';
  header.file.magic[1] = 'M';
  header.file.reserved0 = 0;
  header.file.data_offset = sizeof(bmp_header_t);
  header.file.file_size = header.file.data_offset + header.bitmap.data_size;

  return header;
}

bmp_read_status_t image_read_bmp(FILE* source_bmp, image_t* image) {
  bmp_header_t header;
  fread(&header, 1, sizeof(bmp_header_t), source_bmp);

  if (header.file.magic[0] != 'B' || header.file.magic[1] != 'M')
    return READ_INVALID_SIGNATURE;

  if (header.bitmap.bits_per_pixel != 24)
    return READ_INVALID_BITS;

  uint8_t* raw_data = (uint8_t*) malloc(header.bitmap.data_size);

  fseek(source_bmp, header.file.data_offset, SEEK_SET);
  fread(raw_data, header.bitmap.data_size, 1, source_bmp);

  if (header.bitmap.width % 4 == 0)
    image->data = (pixel_t*) raw_data;
  else {
    pixel_t* unpadded = (pixel_t*) malloc(header.bitmap.width * header.bitmap.height * sizeof(pixel_t));

    for (uint64_t h = 0; h < header.bitmap.height; h++) {
      /* Padding accumulated since the first row = num of rows * padding per row */
      uint64_t padding = h * (header.bitmap.width % 4);
      for (uint64_t w = 0; w < header.bitmap.width; w++) {
        uint64_t pixel_i = h * header.bitmap.width + w;
        unpadded[pixel_i] = *(pixel_t*) (raw_data + sizeof(pixel_t) * pixel_i + padding);
      }
    }

    free(raw_data);
    image->data = unpadded;
  }

  image->width = header.bitmap.width;
  image->height = header.bitmap.height;

  return READ_OK;
}

bmp_write_status_t image_write_bmp(FILE* dest_bmp, const image_t image) {
  bmp_header_t header = image_create_bmp_header(image);
  fwrite(&header, sizeof(bmp_header_t), 1, dest_bmp);

  if (sizeof(bmp_header_t) < header.file.data_offset) {
    uint32_t offset = header.file.data_offset - sizeof(bmp_header_t);
    char* padding = calloc(offset, sizeof(char));
    fwrite(padding, sizeof(char), offset, dest_bmp);
  }

  if (image.width % 4 == 0)
    fwrite(image.data, 1, image.width * image.height * sizeof(pixel_t), dest_bmp);
  else {
    uint64_t data_size = image.width * image.height * sizeof(pixel_t) + image.height * (image.width % 4);
    uint8_t* data = (uint8_t*) calloc(1, data_size);
    for (uint64_t h = 0; h < image.height; h++) {
      uint64_t padding = h * (image.width % 4);
      for (uint64_t w = 0; w < image.width; w++) {
        uint64_t pixel_i = h * image.width + w;
        *((pixel_t*) (data + sizeof(pixel_t) * pixel_i + padding)) = image.data[pixel_i];
      }
    }
    fwrite(data, 1, data_size, dest_bmp);
    free(data);
  }

  fflush(dest_bmp);

  return WRITE_OK;
}
