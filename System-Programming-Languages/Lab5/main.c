#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct __attribute__((packed)) {
  char magic_number[2];
  uint32_t file_size;
  uint32_t reserved0;
  uint32_t data_offset;

  /* DIB */
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
} bmp_header;

typedef struct {
  uint8_t b, g, r;
} pixel;

typedef struct {
  uint64_t width, height;
  pixel* data;
} image;

image image_rotate_180_copy(const image img) {
  pixel* rot_data = (pixel*) malloc(img.width * img.height * sizeof(pixel));
  for (uint64_t h = 0; h < img.height; h++) {
    for (uint64_t w = 0; w < img.width; w++) {
      rot_data[h * img.width + w] = img.data[(img.height - 1 - h) * img.width + w];
    }
  }
  return (image) { img.width, img.height, rot_data };
}

int main() {
  bmp_header header;

  FILE *f = fopen("lain.bmp", "rb");

  fread(&header, 1, sizeof(header), f);
  printf("Magic: %c%c\n", header.magic_number[0], header.magic_number[1]);
  printf("Width: %d, height: %d\n", header.width, header.height);

  uint8_t* raw_data = (uint8_t *) malloc(header.data_size);

  fseek(f, header.data_offset, SEEK_SET);
  fread(raw_data, header.data_size, 1, f);
  fclose(f);

  pixel* img_data;
  if (header.width * header.height * sizeof(pixel) != header.data_size) {
    pixel *new_data = (pixel*) malloc(header.width * header.height * sizeof(pixel));
    for (uint64_t h = 0; h < header.height; h++) {
      for (uint64_t w = 0; w < header.width; w++) {
        uint64_t index = h * header.width + w;
        new_data[index].b = *(raw_data + index * 3 + h * (header.width % 4));
        new_data[index].g = *(raw_data + index * 3 + h * (header.width % 4) + 1);
        new_data[index].r = *(raw_data + index * 3 + h * (header.width % 4) + 2);
      }
    }
    free(raw_data);
    img_data = new_data;
  }
  else {
    img_data = (pixel*) raw_data;
  }

  image img = (image) { header.width, header.height, img_data };

  image rot_img = image_rotate_180_copy(img);
  FILE *new_f = fopen("lain180.bmp", "wb");
  fwrite(&header, sizeof(header), 1, new_f);

  if (sizeof(header) < header.data_offset) {
    uint32_t offset = header.data_offset - sizeof(header);
    char* pad = calloc(offset, sizeof(char));
    fwrite(pad, sizeof(char), offset, new_f);
  }

  if (header.width * header.height * sizeof(pixel) != header.data_size) {
    uint8_t *data = (uint8_t*) calloc(1, header.width * header.height * sizeof(pixel) + header.height * (header.width % 4));
    for (uint64_t h = 0; h < header.height; h++)
      for (uint64_t w = 0; w < header.width; w++) {
        uint64_t index = h * header.width + w;
        data[index * 3 + h * (header.width % 4) + 0] = rot_img.data[index].b;
        data[index * 3 + h * (header.width % 4) + 1] = rot_img.data[index].g;
        data[index * 3 + h * (header.width % 4) + 2] = rot_img.data[index].r;
      }
    free(rot_img.data);
    rot_img.data = (pixel*) data;
  }

  fwrite(rot_img.data, 1, header.data_size, new_f);
  fflush(new_f);
  fclose(new_f);

  return 0;
}
