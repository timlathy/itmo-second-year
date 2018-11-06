#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory.h>

#include "image_bmp_fused.h"

int compute_total_padding_rotated(bmp_header_t* header) {
  return header->bitmap.width * (header->bitmap.height % 4);
}

void rotate_right_padded(const uint8_t* src, uint8_t* dst, uint32_t width, uint32_t height) {
  for (uint32_t h = 0; h < height; h++) {
    uint64_t src_padding = h * (width % 4);

    for (uint32_t w = 0; w < width; w++) {
       uint64_t dst_padding = (width - w) * (height % 4);

       *(pixel_t*)(dst + (((width - 1 - w) * height) + h) * sizeof(pixel_t) + dst_padding) =
         *(pixel_t*)(src + (h * width + w) * sizeof(pixel_t) + src_padding);
    }
  }
}

void image_bmp_fused_rotate_90cw(const char* src_path, const char* dst_path) {
  struct stat src_stat;
  int src_fd = open(src_path, O_RDONLY);
  int dst_fd = open(dst_path, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0644);

  fstat(src_fd, &src_stat);
  size_t src_size = src_stat.st_size;

  uint8_t* src = mmap(NULL, src_size, PROT_READ, MAP_SHARED, src_fd, 0);
  bmp_header_t header = *((bmp_header_t*) src);

  uint32_t width = header.bitmap.width;
  uint32_t height = header.bitmap.height;
  size_t dst_size = header.file.data_offset +
    width * height * sizeof(pixel_t) + compute_total_padding_rotated(&header);

  /* "Stretch" the destination to its full size */
  lseek(dst_fd, dst_size - 1, SEEK_SET);
  write(dst_fd, "", 1);

  uint8_t* dst = mmap(NULL, dst_size, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0);

  rotate_right_padded(src + header.file.data_offset, dst + header.file.data_offset, width, height);

  header.bitmap.height = width;
  header.bitmap.width = height;
  memcpy(dst, &header, sizeof(bmp_header_t));

  msync(dst, dst_size, MS_SYNC);
  munmap(src, src_size);
  munmap(dst, src_size);

  close(src_fd);
  close(dst_fd);
}
