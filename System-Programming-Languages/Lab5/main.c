#include <assert.h>

#include "image.h"
#include "image_bmp.h"

int main() {
  FILE* in = fopen("lain.bmp", "rb");

  image_t input_image;

  assert(image_read_bmp(in, &input_image) == READ_OK);
  fclose(in);

  image_t image_rot = image_rotate_copy_90cw(input_image);

  FILE* out = fopen("lain90.bmp", "wb");

  assert(image_write_bmp(out, image_rot) == WRITE_OK);
  fclose(out);

  return 0;
}
