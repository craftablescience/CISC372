#pragma once

#include <stdint.h>

typedef struct {
    uint8_t* data;
    int width;
    int height;
    int bpp;
} Image;

typedef enum {
    EDGE          = 0,
    SHARPEN       = 1,
    BLUR          = 2,
    GAUSSIAN_BLUR = 3,
    EMBOSS        = 4,
    IDENTITY      = 5,
} KernelType;

typedef double Matrix[3][3];
