#ifndef IMAGE_SEPIA_H
#define IMAGE_SEPIA_H

#include "image.h"

void sepia_naive_inplace(image_t* img);
void sepia_sse_inplace(image_t* img);

#endif
