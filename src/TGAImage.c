#include <stdint.h>
#include <stdlib.h>

#include <TGAImage.h>
#include "Private/TGAPrivate.h"

TGAError tga_err = TGA_NO_ERR;
char tga_err_string[TGA_ERR_MAX] = {0};

static int _allocate_tga_data(TGAImage *image, uint8_t depth,
                              uint16_t width, uint16_t height)
{
    int bytes = (depth + 7)/8;
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGA Image.");
    image->data = malloc(bytes * sizeof(uint8_t) * width * height);
    check(image->data, TGA_MEM_ERR, "Out of memory.");
    return 1;

error:
    if(image->data)
        free(image->data);
    image->data = NULL;
    return 0;
}

/* TODO: Refactor to use the Color Type. */
TGAImage *new_tga_image(TGAColorType ct, uint8_t depth,
                        uint16_t width, uint16_t height)
{
    TGAImage *image = malloc(sizeof(TGAImage));
    check(image, TGA_MEM_ERR, "Unable to allocate memory for new TGAImage.");
    image->_meta = malloc(sizeof(struct _NY_TgaMeta));
    check(image->_meta, TGA_MEM_ERR, "Unable to allocate memory for TGA Metadata.");
    image->id_field = NULL;
    image->version = 2;
    memset(image->__padding, '\0', sizeof(image->__padding));

    if(ct != TGA_NO_DATA)
        if(!_allocate_tga_data(image, depth, width, height))
            goto error; /* allocate will have set err already. */

    image->_meta->extension_offset = 0;
    image->_meta->developer_offset = 0;
    image->_meta->c_map_length = 0;
    image->_meta->x_offset = 0;
    image->_meta->y_offset = 0;
    image->_meta->width = 0;
    image->_meta->height = 0;
    image->_meta->c_map_start = 0;
    image->_meta->id_length = 0;
    image->_meta->c_map_type = 0;
    image->_meta->image_type = 0;
    image->_meta->pixel_depth = 0;
    image->_meta->c_map_depth = 0;
    image->_meta->image_descriptor = 0;
    memset(image->_meta->__padding, '\0', sizeof(image->_meta->__padding));

    return image;

error:
    if(image)
    {
        if(image->_meta)
            free(image->_meta);
        if(image->id_field)
            free(image->id_field);
        free(image);
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

uint8_t tga_get_id_field_length(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->id_length;
error:
    return 0;
}

uint8_t tga_has_color_map(TGAImage *image)
{
    return image->_meta->c_map_type == TGA_COLOR_MAPPED ||
           image->_meta->c_map_type == TGA_ENCODED_COLOR_MAPPED;
}

uint8_t tga_get_color_map_type(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->c_map_type;
error:
    return 0;
}

TGAColorType tga_get_image_type(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    switch(image->_meta->image_type)
    {
        case TGA_NO_DATA: return TGA_NO_DATA;
        case TGA_COLOR_MAPPED: return TGA_COLOR_MAPPED;
        case TGA_TRUECOLOR: return TGA_TRUECOLOR;
        case TGA_MONOCHROME: return TGA_MONOCHROME;
        case TGA_ENCODED_COLOR_MAPPED: return TGA_ENCODED_COLOR_MAPPED;
        case TGA_ENCODED_TRUECOLOR: return TGA_ENCODED_TRUECOLOR;
        case TGA_ENCODED_MONOCHROME: return TGA_ENCODED_MONOCHROME;
        default: return TGA_UNKNOWN_TYPE;
    }
error:
    return TGA_NO_DATA;
}

uint16_t tga_get_color_map_start(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->c_map_start;
error:
    return 0;
}

uint16_t tga_get_color_map_length(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->c_map_length;
error:
    return 0;
}

uint8_t tga_get_color_map_depth(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->c_map_depth;
error:
    return 0;
}

uint16_t tga_get_x_offset(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->x_offset;
error:
    return 0;
}

uint16_t tga_get_y_offset(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->y_offset;
error:
    return 0;
}

uint16_t tga_get_width(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->width;
error:
    return 0;
}

uint16_t tga_get_height(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->height;
error:
    return 0;
}

uint8_t tga_get_pixel_depth(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->pixel_depth;
error:
    return 0;
}

uint32_t tga_get_extension_offset(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return 0;
error:
    return image->_meta->extension_offset;
}

uint32_t tga_get_developer_offset(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return 0;
error:
    return image->_meta->developer_offset;
}

void tga_get_origin_coordinates(TGAImage *image, int *x, int *y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    *y = (32 & image->_meta->image_descriptor) > 0; /*00100000*/
    *x = (16 & image->_meta->image_descriptor) > 0; /*00010000*/
error:
    *x = *y = 0;
}

uint8_t tga_get_attribute_bits(TGAImage *image)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    return image->_meta->image_descriptor & (uint8_t)15; /*00001111*/
error:
    return 0;
}

uint8_t tga_is_monochrome(TGAImage *image)
{
    if((tga_get_image_type(image) == TGA_MONOCHROME ||
            tga_get_image_type(image) == TGA_ENCODED_MONOCHROME ) &&
            tga_get_pixel_depth(image) == 8)
        return 1;
    return 0;
}

/* TODO: Implement for all pixel depths and TGA Types */
/* IMPLEMENTED: TGA_TRUECOLOR, TGA_MONOCHROME */
static uint8_t *_get_pixel_point_at(TGAImage *image, uint16_t x, uint16_t y)
{
    uint8_t depth = (uint8_t)((tga_get_pixel_depth(image)+7)/8);
    uint32_t row_width = tga_get_width(image)*depth;
    return image->data + (y * row_width) + (x * depth);
}

uint8_t tga_get_red_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get red channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 8:
            return pixel[0];
        case 16:
            /* For 16-bit we'll assume the image was stored thusly:
             * ABBBBBGG GGGRRRRR */
            pixel++;
            return (*pixel) & (uint8_t)31; /* VALUE & 00011111 */
        case 24:
            return pixel[2];
        case 32:
            return pixel[2];
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
error:
    return 0;
}

uint8_t tga_get_green_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    uint8_t value = 0; /* For the 16-bit case */
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get green channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 8:
            return pixel[0];
        case 16:
            /* For 16-bit we'll assume the image was stored thusly:
             * ABBBBBGG GGGRRRRR */
            value += ((*pixel++) & (uint8_t)3) << 3; /* pixel & 00000011 */
            value += ((*pixel) & (uint8_t)(224)) >> 5; /* pixel & 11100000 */
            return value;
        case 24:
            return pixel[1];
        case 32:
            return pixel[1];
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
error:
    return 0;
}

uint8_t tga_get_blue_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get blue channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* For 16-bit we'll assume the image was stored thusly:
             * ABBBBBGG GGGRRRRR */
            return ((*pixel) & (uint8_t)124) >> 2; /* VALUE & 01111100 */
        case 24:
            return pixel[0];
        case 32:
            return pixel[1];
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
error:
    return 0;
}

uint8_t tga_get_alpha_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get alpha on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* Assuming ABBBBBGGGGGRRRRR */
            return ((*pixel)&(uint8_t)128) >> 7;
        case 24:
            return 0;
        case 32:
            return pixel[0];
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_get_mono_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(!tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Not a monochrome image.");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    return *pixel;
    return 1;
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_set_red_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t red)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set red channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            pixel[1] |= (red & 31); // GGGRRRRR | (VALUE & 00011111)
            break;
        case 24:
            pixel[2] = red;
            break;
        case 32:
            pixel[3] = red;
            break;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
    return 1;
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_set_green_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t green)
{

    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set green channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* ABBBBBGG GGGRRRRR */
            pixel[0] |= ((green & 24) >> 3); /* ABBBBBGG | (GREEN & 00011000) */
            pixel[1] |= ((green & 7) << 5);  /* GGGRRRRR | (GREEN & 00000111) */
            break;
        case 24:
            pixel[1] = green;
            break;
        case 32:
            pixel[2] = green;
            break;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
    return 1;
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_set_blue_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t blue)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set blue channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* ABBBBBGG GGGRRRRR */
            pixel[0] |= ((blue & 31) << 2);
        case 24:
            pixel[0] = blue;
        case 32:
            pixel[1] = blue;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }

    return 1;
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_set_alpha_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t alpha)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set alpha channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* ABBBBBGG GGGRRRRR */
            if(alpha)
                pixel[0] |= 128; /* PIXEL | 10000000 */
            else
                pixel[0] &= 127; /* PIXEL & 01111111 */
        case 32:
            pixel[0] = alpha;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }

    return 1;
error:
    return 0;
}

/* TODO: Implement support for ColorMapped images. */
uint8_t tga_set_mono_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t mono)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Passed.");
    if(!tga_is_monochrome(image))
        fail(TGA_TYPE_ERR,"Can't set monochrome value on non-monochrome image");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    *pixel = mono;
    return 1;
error:
    return 0;
}

TGAError tga_error(void)
{
    return tga_err;
}

char *tga_error_str(void)
{
    return tga_err_string;
}

void tga_clear_error(void)
{
    tga_err = TGA_NO_ERR;
    memset(tga_err_string, 0, TGA_ERR_MAX);
}
