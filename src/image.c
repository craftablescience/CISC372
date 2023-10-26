#include "image.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define IMG_DATA_INDEX(x, y, width, bit, bpp) \
    (y * width * bpp + bpp * x + bit)

//An array of kernel matrices to be used for image convolution.  
//The indexes of these match the enumeration from the header file. ie. ALGORITHMS[BLUR] returns the kernel corresponding to a box blur.
const Matrix ALGORITHMS[] = {
        // EDGE
    {
        { 0,-1, 0},
        {-1, 4,-1},
        { 0,-1, 0},
        },
        // SHARPEN
    {
        { 0,-1, 0},
        {-1, 5,-1},
        { 0,-1, 0},
        },
        // BLUR
    {
        {1/9.0,1/9.0,1/9.0},
        {1/9.0,1/9.0,1/9.0},
        {1/9.0,1/9.0,1/9.0},
        },
        // GAUSSIAN_BLUR
    {
        {1.0/16,1.0/8,1.0/16},
        { 1.0/8,1.0/4, 1.0/8},
        {1.0/16,1.0/8,1.0/16},
        },
        // EMBOSS
    {
        {-2,-1,0},
        {-1, 1,1},
        { 0, 1,2},
        },
        // IDENTITY
    {
        {0,0,0},
        {0,1,0},
        {0,0,0},
    },
};

/*
 * getPixelValue: Computes the value of a specific pixel on a specific channel using the selected convolution kernel
 * Parameters:
 *     srcImage: An Image struct populated with the image being convoluted
 *     x: The x coordinate of the pixel
 *     y: The y coordinate of the pixel
 *     bit: The color channel being manipulated
 *     algorithm: The 3x3 kernel matrix to use for the convolution
 * Returns:
 *     The new value for this x,y pixel and bit channel
 */
static uint8_t getPixelValue(Image* srcImage, int x, int y, int bit, const Matrix algorithm) {
    // for the edge pixes, just reuse the edge pixel
    int px = x + 1;
    int py = y + 1;
    int mx = x - 1;
    int my = y - 1;

    if (mx < 0) mx = 0;
    if (my < 0) my = 0;
    if (px >= srcImage->width) px = srcImage->width - 1;
    if (py >= srcImage->height) py = srcImage->height - 1;

    return
        algorithm[0][0] * srcImage->data[IMG_DATA_INDEX(mx, my, srcImage->width, bit, srcImage->bpp)] +
        algorithm[0][1] * srcImage->data[IMG_DATA_INDEX(x,  my, srcImage->width, bit, srcImage->bpp)] +
        algorithm[0][2] * srcImage->data[IMG_DATA_INDEX(px, my, srcImage->width, bit, srcImage->bpp)] +
        algorithm[1][0] * srcImage->data[IMG_DATA_INDEX(mx, y,  srcImage->width, bit, srcImage->bpp)] +
        algorithm[1][1] * srcImage->data[IMG_DATA_INDEX(x,  y,  srcImage->width, bit, srcImage->bpp)] +
        algorithm[1][2] * srcImage->data[IMG_DATA_INDEX(px, y,  srcImage->width, bit, srcImage->bpp)] +
        algorithm[2][0] * srcImage->data[IMG_DATA_INDEX(mx, py, srcImage->width, bit, srcImage->bpp)] +
        algorithm[2][1] * srcImage->data[IMG_DATA_INDEX(x,  py, srcImage->width, bit, srcImage->bpp)] +
        algorithm[2][2] * srcImage->data[IMG_DATA_INDEX(px, py, srcImage->width, bit, srcImage->bpp)];
}

/*
 * convolute: Applies a kernel matrix to an image
 * Parameters:
 *     srcImage: The image being convoluted
 *     destImage: A pointer to a  pre-allocated (including space for the pixel array) structure to receive the convoluted image.
 *                It should be the same size as srcImage
 *     algorithm: The kernel matrix to use for the convolution
 */
static void convolute(Image* srcImage, Image* destImage, const Matrix algorithm) {
    if (srcImage->width != destImage->width || srcImage->height != destImage->height || srcImage->bpp != destImage->bpp)
        return;
    for (int row = 0; row < srcImage->height; row++){
        for (int pix = 0; pix < srcImage->width; pix++){
            for (int bit = 0; bit < srcImage->bpp; bit++){
                destImage->data[IMG_DATA_INDEX(pix, row, srcImage->width, bit, srcImage->bpp)] = getPixelValue(srcImage, pix, row, bit, algorithm);
            }
        }
    }
}

/*
 * getKernelType: Converts the string name of a convolution into a value from the KernelTypes enumeration
 * Parameters:
 *     type: A string representation of the type
 * Returns:
 *     An appropriate entry from the KernelTypes enumeration. Defaults to IDENTITY, which does nothing but copy the image.
 */
static KernelType getKernelType(char* type) {
    if (!strcmp(type, "edge")) return EDGE;
    else if (!strcmp(type, "sharpen")) return SHARPEN;
    else if (!strcmp(type, "blur")) return BLUR;
    else if (!strcmp(type, "gauss")) return GAUSSIAN_BLUR;
    else if (!strcmp(type, "emboss")) return EMBOSS;
    else return IDENTITY;
}

/*
 * extractFileName: Get the file name from a path
 * Parameters:
 *     path: The path to the file
 * Returns:
 *     A pointer to the provided string beginning at the filename
 */
static char* extractFileName(char* path) {
    char* out;
    (out = strrchr(path, '/')) ? ++out : (out = path);
    return out;
}

/*
 * printAndReturn: Prints something to the console and returns
 * Parameters:
 *     str: The string to format
 *     ...: The values to apply to the format string
 * Returns:
 *     EXIT_FAILURE
 */
static int printAndReturn(const char* str, ...) {
    va_list(args);
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    return EXIT_FAILURE;
}

/*
 * main: Main function
 * Parameters:
 *     argc: Expected to be 3 (program name, arg1, arg2)
 *     argv[1]: The source file name (can be jpg, png, bmp, tga)
 *     argv[2]: The name of the algorithm
 */
int main(int argc, char** argv) {
    if (argc != 3)
        return printAndReturn("%s", "Usage: image <filename> <type>\n\twhere type is one of (edge,sharpen,blur,gauss,emboss,identity)\n");;

    // Start timer
    time_t timeStart = time(NULL);

    // Get arguments
    char* fileName = argv[1];
    KernelType type = getKernelType(argv[2]);

    // Get output filename
    char outputName[PATH_MAX];
    strcpy(outputName, argv[2]);
    outputName[strlen(argv[2])] = '_';
    strcpy(outputName + strlen(argv[2]) + 1, extractFileName(fileName));

    // Don't flip images, we're not a graphics library
    stbi_set_flip_vertically_on_load(0);

    // Load source image
    Image srcImage;
    srcImage.data = stbi_load(fileName, &srcImage.width, &srcImage.height, &srcImage.bpp, 0);
    if (!srcImage.data)
        return printAndReturn("Error loading file \"%s\"!\n", fileName);

    // Create destination image
    Image destImage;
    destImage.bpp = srcImage.bpp;
    destImage.height = srcImage.height;
    destImage.width = srcImage.width;
    destImage.data = malloc(sizeof(uint8_t) * destImage.width * destImage.bpp * destImage.height);

    // Process source image
    convolute(&srcImage, &destImage, ALGORITHMS[type]);
    stbi_write_png(outputName, destImage.width, destImage.height, destImage.bpp, destImage.data, destImage.bpp * destImage.width);

    // Free data
    free(destImage.data);
    stbi_image_free(srcImage.data);

    // End timer
    time_t timeEnd = time(NULL);
    printf("Took %ld seconds\n", timeEnd - timeStart);

    return EXIT_SUCCESS;
}
