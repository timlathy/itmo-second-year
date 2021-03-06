#include <assert.h>
#include <string.h>

#include "image.h"
#include "image_bmp.h"
#include "image_bmp_fused.h"

#define PANIC(...) do { fprintf(stderr, __VA_ARGS__); return 1; } while (0)

int main(int argc, char** argv) {
  if (argc != 4) PANIC("Usage: lab5 (fread | mmap) source.bmp destination.bmp\n");

  bool use_mmap;
  if (strcmp(argv[1], "fread") == 0) use_mmap = false;
  else if (strcmp(argv[1], "mmap") == 0) use_mmap = true;
  else PANIC("Usage: lab5 (fread | mmap) source.bmp destination.bmp\n");

  const char* input_path = argv[2];
  const char* output_path = argv[3];

  if (use_mmap) {
    image_bmp_fused_rotate_90cw(input_path, output_path);
    return 0;
  }

  FILE* input_file = fopen(input_path, "rb");
  if (input_file == NULL) PANIC("Unable to open %s for reading\n", input_path);

  image_t input_image;

  bmp_read_status_t read_status = image_read_bmp(input_file, &input_image);
  fclose(input_file);

  switch (read_status) {
    case READ_INVALID_SIGNATURE:
      PANIC("%s is not a valid BMP file\n", input_path);
    case READ_UNSUPPORTED_BITS_PER_PIXEL:
      PANIC("Only 24-bit (8 bits per channel) source images are supported\n");
    case READ_ERROR:
      PANIC("Encountered an error while opening %s\n", input_path);
    case READ_OK:
      break;
  }

  image_t image_rot = image_rotate_copy_90cw(input_image);

  FILE* output_file = fopen(output_path, "wb");
  if (output_file == NULL) PANIC("Unable to open %s for writing\n", output_path);

  bmp_write_status_t write_status = image_write_bmp(output_file, image_rot);
  fclose(output_file);

  switch (write_status) {
    case WRITE_ERROR:
      PANIC("Encountered an error while saving the destination image\n");
    case WRITE_OK:
      return 0;
  }
}
