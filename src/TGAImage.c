#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <TGAImage.h>

#include "Private/TGAPrivate.h"

static int _allocate_tga_data(TGAImage *image, ChannelType ct,
        uint16_t width, uint16_t height)
{
    if(!_tga_sanity(image))
        goto error;
    switch(ct)
    {
        case CT_MONOCHROME:
            image->data = malloc(sizeof(uint8_t) * width * height);
            break;
        case CT_R5G5B5A:
        case CT_AR5G5B5:
        case CT_B5G5R5A:
        case CT_AB5G5R5:
            image->data = malloc(2 * sizeof(uint8_t) * width * height);
            break;
        case CT_RGB:
        case CT_BGR:
            image->data = malloc(3 * sizeof(uint8_t) * width * height);
            break;
        case CT_RGBA:
        case CT_ABGR:
        case CT_ARGB:
        case CT_BGRA:
            image->data = malloc(4 * sizeof(uint8_t) * width * height);
            break;
        case CT_RGBA64:
            image->data = malloc(8 * sizeof(uint8_t) * width * height);
            break;
        default:
            sentinel("Unsupported TGA Channel Type.");
    }
    check(image->data, "Out of memory.");

error:
    if(image->data)
        free(image->data);
    image->data = NULL;
    return 0;
}

TGAImage *new_tga_image(ChannelType channels, uint16_t width, uint16_t height)
{
    TGAImage *img = malloc(sizeof(TGAImage));
    check(img, "Unable to allocate memory for new TGAImage.");
    img->_meta = malloc(sizeof(struct _NY_TgaMeta));
    check(img->_meta, "Unable to allocate memory for TGA Metadata.");
    img->id_field = NULL;
    img->version = 2;
    memset(img->__padding, '\0', sizeof(img->__padding));

    if(channels != CT_NONE && channels != CT_UNKNOWN && channels != CT_CUSTOM)
    {
        check(_allocate_tga_data(img, channels, width, height),
                "Unable to allocate image data.");
    }
    else
        img->data = NULL;

    img->_meta->extension_offset = 0;
    img->_meta->developer_offset = 0;
    img->_meta->c_map_length = 0;
    img->_meta->x_offset = 0;
    img->_meta->y_offset = 0;
    img->_meta->width = 0;
    img->_meta->height = 0;
    img->_meta->c_map_start = 0;
    img->_meta->id_length = 0;
    img->_meta->c_map_type = CT_NONE;
    img->_meta->image_type = CT_NONE;
    img->_meta->pixel_depth = 0;
    img->_meta->c_map_depth = 0;
    img->_meta->image_descriptor = 0;
    memset(img->_meta->__padding, '\0', sizeof(img->_meta->__padding));

    return img;

error:
    if(img)
    {
        if(img->_meta)
            free(img->_meta);
        if(img->id_field)
            free(img->id_field);
        free(img);
    }
    return NULL;
}

void free_tga_image(TGAImage *image)
{
    if(image)
    {
        if(image->_meta)
            free(image->_meta);
        if(image->id_field)
            free(image->id_field);
        if(image->data)
            free(image->data);
        free(image);
    }
}

Image *tga_to_generic(TGAImage *img)
{
    Image *gen_image = malloc(sizeof(Image));
    check(img, "Image Passed was Null.");
    check(img->_meta, "TGA Image lacks metadata.");

    return gen_image;

error:
    if(gen_image)
        free(gen_image);
    return NULL;
}

uint8_t tga_get_id_field_length(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->id_length;
    return 0;
}

uint8_t tga_has_color_map(TGAImage *image)
{
    return tga_get_color_map_type(image);
}

uint8_t tga_get_color_map_type(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->c_map_type;
    return 0;
}

TGAColorType tga_get_image_type(TGAImage *image)
{
    if(!_tga_sanity(image))
        return TGA_NO_DATA;
    switch(image->_meta->image_type)
    {
        case TGA_NO_DATA: return TGA_NO_DATA;
        case TGA_COLOR_MAPPED: return TGA_COLOR_MAPPED;
        case TGA_TRUECOLOR: return TGA_TRUECOLOR;
        case TGA_MONOCHROME: return TGA_MONOCHROME;
        case TGA_ENCODED_COLOR_MAPPED: return TGA_ENCODED_COLOR_MAPPED;
        case TGA_ENCODED_TRUECOLOR: return TGA_ENCODED_TRUECOLOR;
        case TGA_ENCODED_MONOCHROME: return TGA_ENCODED_MONOCHROME;
        default: return TGA_INVALID_TYPE;
    }
}

uint16_t tga_get_color_map_start(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->c_map_start;
    return 0;
}

uint16_t tga_get_color_map_length(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->c_map_length;
    return 0;
}

uint8_t tga_get_color_map_depth(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->c_map_depth;
    return 0;
}

uint16_t tga_get_x_offset(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->x_offset;
    return 0;
}

uint16_t tga_get_y_offset(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->y_offset;
    return 0;
}

uint16_t tga_get_width(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->width;
    return 0;
}

uint16_t tga_get_height(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->height;
    return 0;
}

uint8_t tga_get_pixel_depth(TGAImage *image)
{
    if(_tga_sanity(image))
        return image->_meta->pixel_depth;
    return 0;
}

uint32_t tga_get_extension_offset(TGAImage *img)
{
    if(!_tga_sanity(img) || img->version == 1)
        return 0;
    return img->_meta->extension_offset;
}

uint32_t tga_get_developer_offset(TGAImage *img)
{
    if(!_tga_sanity(img) || img->version == 1)
        return 0;
    return img->_meta->developer_offset;
}

void tga_get_origin_coordinates(TGAImage *image, int *x, int *y)
{
    if(_tga_sanity(image))
    {
        *x = (8 & image->_meta->image_descriptor) > 0; /*00001000*/
        *y = (4 & image->_meta->image_descriptor) > 0; /*00000100*/
    }
    else
    {
        *x = 0;
        *y = 0;
    }
}

uint8_t tga_get_attribute_bits(TGAImage *image)
{
    if(!_tga_sanity(image))
        return 0;
    return (image->_meta->image_descriptor & (240)) >> 4; /*11110000*/
}

static uint8_t *_get_pixel_point_at(TGAImage *image, uint16_t x, uint16_t y)
{
    uint8_t depth_mult = image->_meta->pixel_depth/8;
    if(image->_meta->pixel_depth % 8 != 0)
        depth_mult++;
    uint32_t row_width = image->_meta->width * depth_mult;
    return image->data + (y * row_width) + (x * depth_mult);
}

uint8_t tga_get_red_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), "Problem getting red channel.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(image->_meta->pixel_depth)
    {
        case 8:
            return pixel[0];
        case 24:
            return pixel[2];
        case 32:
            return pixel[2];
        default:
            sentinel("Unsupported pixel depth.");
    }
error:
    return 0;
}

uint8_t tga_get_green_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), "Problem getting green channel.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(image->_meta->pixel_depth)
    {
        case 8:
            return pixel[0];
        case 24:
            return pixel[1];
        case 32:
            return pixel[1];
        default:
            sentinel("Unsupported pixel depth.");
    }
error:
    return 0;
}

uint8_t tga_get_blue_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), "Problem getting blue channel.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(image->_meta->pixel_depth)
    {
        case 8:
            return pixel[0];
        case 24:
            return pixel[0];
        case 32:
            return pixel[0];
        default:
            sentinel("Unsupported pixel depth.");
    }
error:
    return 0;
}
