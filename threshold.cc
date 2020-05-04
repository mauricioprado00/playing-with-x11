/*
 * Converts a ppm image into a grayscale and a thresholded version of it
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>

/**
 * Save image to a PPM image file format
 * @see https://en.wikipedia.org/wiki/Netpbm#File_formats
 */
void save_ppm(XImage *pImage, char * filename) {
    FILE *fp;
    int i, j;
    char *pointer;

    fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", pImage->width, pImage->height);
    fprintf(fp, "255\n");

    pointer = pImage->data;
    for (i = 0; i < pImage->width; i++) {
        for (j = 0; j < pImage->height; j++) {
            fprintf(fp, "%hhu %hhu %hhu\n", *(char *)(pointer+2), *(char *)(pointer + 1), *(char *)(pointer + 0));
            // move to next pixel
            pointer += 4;
        }
    }
}

XImage * load_ppm(char *filename)
{
    XImage *grayscale = (XImage *)malloc(sizeof(XImage));
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    unsigned int read;
    unsigned int pixel_max;

    fp = fopen(filename, "r");
    if (fp == NULL){
        exit(4);
    }

    line = (char *) malloc(255);
    
    // check image format
    fscanf(fp, "%s\n", line);
    printf("image format: %s\n", line);
    if (strcmp("P3", line) != 0) {
        printf("image format not supported");
        exit(5);
    }
    free(line);

    // retrieve image size
    fscanf(fp, "%d %d\n", &grayscale->width, &grayscale->height);
    printf("image size %d x %d\n", grayscale->width, grayscale->height);

    // retrieve max pixel color value
    fscanf(fp, "%d\n", &pixel_max);
    printf("max color value: %d\n", pixel_max);

    // allocate space for pixels
    grayscale->data = (char *) malloc(grayscale->width * grayscale->height * 4);

    char * target = grayscale->data;

    while ((read = fscanf(fp, "%hhu %hhu %hhu", target + 2, target + 1, target)) != -1) {
        target += 4;
    }

    fclose(fp);

    return grayscale;
}

XImage * image_grayscale_colorimetric(XImage *pImage)
{
    XImage *grayscale = (XImage *)malloc(sizeof(XImage));
    grayscale->width = pImage->width;
    grayscale->height = pImage->height;
    grayscale->data = (char *) malloc(pImage->width * pImage->height * 4);
    char * source = pImage->data;
    char * target = grayscale->data;
    unsigned int offset = 0;
    unsigned char grayvalue;
    int i,j;
    for (i = 0; i < pImage->width; i++) {
        for (j = 0; j < pImage->height; j++) {
            grayvalue = source[offset + 2] * 0.2126 + source[offset +1] * 0.7172 + source[offset +0] * 0.0722;
            target[offset+0] = grayvalue;
            target[offset+1] = grayvalue;
            target[offset+2] = grayvalue;
            // move to next pixel
            offset += 4;
        }
    }

    return grayscale;
}

XImage * image_threshold(XImage *pImage, unsigned int threshold) 
{
    XImage *grayscale = (XImage *)malloc(sizeof(XImage));
    grayscale->width = pImage->width;
    grayscale->height = pImage->height;
    grayscale->data = (char *) malloc(pImage->width * pImage->height * 4);
    char * source = pImage->data;
    char * target = grayscale->data;
    unsigned int offset = 0;
    unsigned char grayvalue;
    int i,j;
    for (i = 0; i < pImage->width; i++) {
        for (j = 0; j < pImage->height; j++) {
            grayvalue = source[offset + 2] * 0.2126 + source[offset +1] * 0.7172 + source[offset +0] * 0.0722;
            if (grayvalue > threshold) { 
                grayvalue = 255;
            }
            target[offset+0] = grayvalue;
            target[offset+1] = grayvalue;
            target[offset+2] = grayvalue;
            // move to next pixel
            offset += 4;
        }
    }

    return grayscale;
}


int main(int argc, char* argv[]) {

    char *filename;
    if (argc < 2) {
        printf("Usage: threshold image.ppm\n");
        return 1;
    }

    unsigned int threshold_value = 0x85;
    if (argc > 2) {
        sscanf(argv[2], "%u", &threshold_value);
        printf("threshold value: %u\n", threshold_value);
    }

    filename = argv[1];

    printf("image file: %s\n", filename);
    if (access(filename, F_OK) == -1) {
        printf("file not accessible\n");
        return 2;
    }

    char output_filename[255];
    XImage *image = load_ppm(filename);

    XImage *grayscale = image_grayscale_colorimetric(image);
    sprintf(output_filename, "grayscale-%s", filename);
    save_ppm(grayscale, output_filename);
    printf("grayscaled saved to %s\n", output_filename);

    XImage *threshold = image_threshold(image, threshold_value);
    sprintf(output_filename, "threshold-%s", filename);
    save_ppm(threshold, output_filename);
    printf("threshold saved to %s\n", output_filename);

    free(image);

    return 0;
}

