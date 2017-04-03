#include <stdlib.h>

#include <Image.h>
#include <TGAImage.h>
#include <Util.h>

void set_rgba_pixel(uint32_t x, uint32_t y, Image* img,
        uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    check(img, "Image is NULL.");
    check(img->data, "Image Data is NULL.");
    check(img->channel_type == CT_RGBA, "Not an RGBA Image.");
    uint64_t offset = 4 * ((y * img->width)  + x);
    uint8_t *pixel_start = img->data+offset;
    pixel_start[0] = red;
    pixel_start[1] = green;
    pixel_start[2] = blue;
    pixel_start[3] = alpha;

error:
    return;
}

void set_rgb_pixel(uint32_t x, uint32_t y, Image* img,
        uint8_t red, uint8_t green, uint8_t blue)
{
    check(img, "Image is NULL.");
    check(img->data, "Image Data is NULL.");
    check(img->channel_type == CT_RGB, "Not an RGB Image.");
    uint64_t offset = 3 * ((y * img->width)  + x);
    uint8_t *pixel_start = img->data+offset;
    pixel_start[0] = red;
    pixel_start[1] = green;
    pixel_start[2] = blue;

error:
    return;
}

void set_monochrome_pixel(uint32_t x, uint32_t y, Image* img, uint8_t color)
{
    check(img, "Image is NULL.");
    check(img->data, "Image Data is NULL.");
    check(img->channel_type == CT_MONOCHROME, "Not an Monochrome Image.");
    uint64_t offset = y*img->width + x;
    img->data[offset] = color;

error:
    return;
}

Image* read_image_file(FILE* file, ImageFormat format)
{
    Image* img = malloc(sizeof(Image));
    switch(format)
    {
        case FORMAT_TGA:
            img = tga_to_generic(read_tga_image(file));
            check(img, "There was a problem reading the TGA File.");
            break;
        default:
            sentinel("Sorry, the supplied format is not yet supported.");
    }

    return img;

    error:
        return NULL;
}
