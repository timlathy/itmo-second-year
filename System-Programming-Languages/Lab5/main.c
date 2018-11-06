#include <assert.h>

#include "image.h"
#include "image_bmp.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: lab5 source.bmp destination.bmp\n");
    return 1;
  }

  const char* input_path = argv[1];
  const char* output_path = argv[2];

  FILE* input_file = fopen(input_path, "rb");
  if (input_file == NULL) {
    fprintf(stderr, "Unable to open %s for reading\n", input_path);
    return 1;
  }

  image_t input_image;

  bmp_read_status_t read_status = image_read_bmp(input_file, &input_image);
  fclose(input_file);

  switch (read_status) {
    case READ_INVALID_SIGNATURE:
      fprintf(stderr, "%s is not a valid BMP file\n", input_path);
      return 1;
    case READ_UNSUPPORTED_BITS_PER_PIXEL:
      fprintf(stderr, "Only 24-bit (8 bits per channel) source images are supported\n");
      return 1;
    case READ_ERROR:
      fprintf(stderr, "Encountered an error while opening %s\n", input_path);
      return 1;
    default:
      break;
  }

  image_t image_rot = image_rotate_copy_90cw(input_image);

  FILE* output_file = fopen(output_path, "wb");
  if (output_file == NULL) {
    fprintf(stderr, "Unable to open %s for writing\n", output_path);
    return 1;
  }

  bmp_write_status_t write_status = image_write_bmp(output_file, image_rot);
  fclose(output_file);

  switch (write_status) {
    case WRITE_ERROR:
      fprintf(stderr, "Encountered an error while saving the destination image\n");
      return 1;
    default:
      break;
  }

  return 0;
}
