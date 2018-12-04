#include <assert.h>
#include <string.h>

#include "image.h"
#include "image_bmp.h"
#include "image_sepia.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#define PANIC(...) do { fprintf(stderr, __VA_ARGS__); return 1; } while (0)

#define BENCHMARK_NUM_RUNS 250UL

void do_benchmark(image_t* img) {
  struct rusage r;
  struct timeval start, end;

  getrusage(RUSAGE_SELF, &r); start = r.ru_utime;

  for (uint64_t i = 0; i < BENCHMARK_NUM_RUNS; i++) sepia_avx_inplace(img);

  getrusage(RUSAGE_SELF, &r); end = r.ru_utime;

  long res = ((end.tv_sec - start.tv_sec) * 1000000L) + end.tv_usec - start.tv_usec;

  printf("AVX implementation: %ld μs (average for %lu runs)\n", res / BENCHMARK_NUM_RUNS, BENCHMARK_NUM_RUNS);

  // ---

  getrusage(RUSAGE_SELF, &r); start = r.ru_utime;

  for (uint64_t i = 0; i < BENCHMARK_NUM_RUNS; i++) sepia_sse_inplace(img);

  getrusage(RUSAGE_SELF, &r); end = r.ru_utime;

  res = ((end.tv_sec - start.tv_sec) * 1000000L) + end.tv_usec - start.tv_usec;

  printf("SSE implementation: %ld μs (average for %lu runs)\n", res / BENCHMARK_NUM_RUNS, BENCHMARK_NUM_RUNS);

  // ---

  getrusage(RUSAGE_SELF, &r); start = r.ru_utime;

  for (uint64_t i = 0; i < BENCHMARK_NUM_RUNS; i++) sepia_naive_inplace(img);

  getrusage(RUSAGE_SELF, &r); end = r.ru_utime;

  res = ((end.tv_sec - start.tv_sec) * 1000000L) + end.tv_usec - start.tv_usec;

  printf("Naive implementation: %ld μs (average for %lu runs)\n", res / BENCHMARK_NUM_RUNS, BENCHMARK_NUM_RUNS);
}

const char* USAGE = "Usage: lab7 (naive|sse|avx|benchmark) src.bmp dst.bmp\n";

int main(int argc, char** argv) {
  if (argc != 4 || (strncmp(argv[1], "naive", 5) != 0
        && strncmp(argv[1], "sse", 3) != 0
        && strncmp(argv[1], "avx", 3) != 0
        && strncmp(argv[1], "benchmark", 9) != 0)) PANIC(USAGE);

  const char* input_path = argv[2];
  const char* output_path = argv[3];

  FILE* input_file = fopen(input_path, "rb");
  if (input_file == NULL) PANIC("Unable to open %s for reading\n", input_path);

  image_t img;

  bmp_read_status_t read_status = image_read_bmp(input_file, &img);
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

  FILE* output_file = fopen(output_path, "wb");
  if (output_file == NULL) PANIC("Unable to open %s for writing\n", output_path);

  if (strncmp(argv[1], "naive", 5) == 0) sepia_naive_inplace(&img);
  else if (strncmp(argv[1], "sse", 3) == 0) sepia_sse_inplace(&img);
  else if (strncmp(argv[1], "avx", 3) == 0) sepia_avx_inplace(&img);
  else {
    do_benchmark(&img);
    return 0;
  }

  bmp_write_status_t write_status = image_write_bmp(output_file, img);
  fclose(output_file);

  switch (write_status) {
    case WRITE_ERROR:
      PANIC("Encountered an error while saving the destination image\n");
    case WRITE_OK:
      return 0;
  }
}
