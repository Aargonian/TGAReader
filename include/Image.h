#ifndef __NY_IMAGE_H
#define __NY_IMAGE_H

#include <stdio.h>
#include <stdint.h>

#include <ImageDefs.h>

void set_rgba_pixel(uint32_t x, uint32_t y, Image* img,
        uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
void set_rgb_pixel(uint32_t x, uint32_t y, Image* img,
        uint8_t red, uint8_t green, uint8_t blue);
void set_monochrome_pixel(uint32_t x, uint32_t y, Image* img, uint8_t color);

Image* read_image_file(FILE* file, ImageFormat format);

#endif/*__NY_IMAGE_H*/
