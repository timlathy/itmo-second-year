#include <assert.h>

#include "image.h"
#include "image_bmp.h"

int main() {
  FILE* in = fopen("lain.bmp", "rb");

  image_t input_image;

  /* FIXME abstraction leak: BMP routines should not expose bmp_header_t
   * image_write_bmp should recreate the header itself */
  bmp_header_t header;

  assert(image_read_bmp(in, &input_image, &header) == READ_OK);
  fclose(in);

  image_t image_rot = image_rotate_copy_90cw(input_image);

  /* FIXME see above */
  header.width = image_rot.width;
  header.height = image_rot.height;

  FILE* out = fopen("lain90.bmp", "wb");

  assert(image_write_bmp(out, image_rot, header) == WRITE_OK);
  fclose(out);

  return 0;
}
