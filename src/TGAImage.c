#include <stdint.h>
#include <stdlib.h>

#include <TGAImage.h>
#include "Private/TGAPrivate.h"

TGAError tga_err = TGA_NO_ERR;
char tga_err_string[TGA_ERR_MAX] = {0};

static int _coordinate_sanity(TGAImage *image, uint16_t x, uint16_t y)
{
    check(x < tga_get_width(image), TGA_ARG_ERR, "X coordinate is larger than"
            " image width. X: %d, WIDTH: %d", x, tga_get_width(image));
    check(y < tga_get_height(image), TGA_ARG_ERR, "Y coordinate is larger "
            "than image height. X: %d, WIDTH: %d", x, tga_get_height(image));
    return 1;
error:
    return 0;
}

static int _allocate_tga_data(TGAImage *image, uint8_t depth,
                              uint16_t width, uint16_t height)
{
    int bytes = (depth + 7)/8;
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGA Image.");
    image->data = malloc(bytes * sizeof(uint8_t) * width * height);
    check(image->data, TGA_MEM_ERR, "Out of memory.");
    memset(image->data, 0, bytes * sizeof(uint8_t) * width * height);
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

    image->_meta->image_type = ct;
    image->_meta->extension_offset = 0;
    image->_meta->developer_offset = 0;
    image->_meta->c_map_length = 0;
    image->_meta->x_offset = 0;
    image->_meta->y_offset = 0;
    image->_meta->width = width;
    image->_meta->height = height;
    image->_meta->c_map_start = 0;
    image->_meta->id_length = 0;
    image->_meta->c_map_type = 0;
    image->_meta->pixel_depth = depth;
    image->_meta->c_map_depth = 0;
    image->_meta->image_descriptor = 0;

    /*if(depth == 16 || depth == 32)
    {
        image->_meta->image_descriptor |= (depth == 16) ? 1 : 15;
    }*/
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get red channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    return ((pixel[1] & 124) >> 2); /* ARRRRRGG & 01111100 */
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    uint8_t value = 0; /* For the 16-bit case */
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get green channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    value += ((pixel[0] & 224) >> 5); /* GGGBBBBB & 11100000 */
	    value += ((pixel[1] & 3) << 3); /* ARRRRRGG & 00000011 */
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get blue channel on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    return pixel[0] & 31; /* GGGBBBBB & 00011111 */
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't get alpha on monochrome image.");
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    return (pixel[1] & 128) > 0; /* ARRRRRGG & 10000000 */
        case 24:
            return 0;
        case 32:
            return pixel[3];
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set red channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    red = red > 31 ? 31 : red; /* Clamp to 0-31 Range for 5-bit. */
	    pixel[1] &= 131; /* ARRRRRGG & 10000011 | Set to 0 */
            pixel[1] |= (red << 2); /* GGGRRRRR | (VALUE & 01111100) */
            break;
        case 24:
            pixel[2] = red;
            break;
        case 32:
            pixel[2] = red;
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set green channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
	    green = green > 31 ? 31 : green; /* Clamp to 0-31 Range for 5-bit */
	    pixel[0] &= 31; /* GGGBBBBB & 00011111 | Set to 0 */
	    pixel[1] &= 252; /* ARRRRRGG & 11111100 | Set to 0 */
            pixel[0] |= ((green & 7) << 5); /* GREEN & 00000111 */
            pixel[1] |= ((green & 24) >> 3); /* GREEN & 00011000 */
            break;
        case 24:
            pixel[1] = green;
            break;
        case 32:
            pixel[1] = green;
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set blue channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
            blue = blue > 31 ? 31 : blue;
	    pixel[0] &= 224; /* GGGBBBBB & 11100000 | Set to 0 */
            pixel[0] |= blue;
	    break;
        case 24:
            pixel[0] = blue;
	    break;
        case 32:
            pixel[0] = blue;
	    break;
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't set alpha channel on monochrome image.");

    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    switch(tga_get_pixel_depth(image))
    {
        case 16:
            /* GGGBBBBB ARRRRRGG*/
            if(alpha)
                pixel[1] |= 128; /* PIXEL | 10000000 */
            else
                pixel[1] &= 127; /* PIXEL & 01111111 */
	    break;
        case 32:
            pixel[3] = alpha;
	    break;
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
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    if(!tga_is_monochrome(image))
        fail(TGA_TYPE_ERR,"Can't set monochrome value on non-monochrome image");
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    *pixel = mono;
    return 1;
error:
    return 0;
}

/* 
 * Primarily made for TrueColor images as a convenience function. Therefore, 
 * error for non-truecolor images. 
 */
uint8_t *tga_create_pixel_for_image(TGAImage *image,
                                    uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    uint8_t depth = (uint8_t)((tga_get_pixel_depth(image) + 7) / 8);
    uint8_t *pixel = malloc(sizeof(uint8_t) * depth);
    memset(pixel, 0, sizeof(uint8_t)*depth);
    if(tga_is_monochrome(image))
        fail(TGA_TYPE_ERR, "Can't create pixel for monochrome image. "
                "Use monochrome functions.");

    switch(depth)
    {
        case 2:
            /* Will set them for: GGGBBBBB ARRRRRGG */
            /* Normalize them for 5-bits each. Guaranteed to be form: 000XXXXX*/
	    r = r > 31 ? 31 : r;
	    g = g > 31 ? 31 : g;
	    b = b > 31 ? 31 : b;

	    if(a)
		pixel[1] |= 128;

	    /* RED */
	    pixel[1] |= r << 2;

	    /* GREEN */
	    pixel[0] |= ((g & 7) << 5);
	    pixel[1] |= ((g & 24) >> 3);

	    /* BLUE */
	    pixel[0] |= b;
            return pixel;

        case 3:
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
	    return pixel;
        case 4:
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
            pixel[3] = a;
            return pixel;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }

error:
    if(pixel)
        free(pixel);
    return NULL;
}

void tga_free_pixel(uint8_t *pixel)
{
    if(pixel)
        free(pixel);
}

uint8_t *tga_get_pixel_copy_at(TGAImage *image, uint16_t x, uint16_t y)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    uint8_t depth = (uint8_t)((tga_get_pixel_depth(image) + 7) / 8);
    uint8_t *pixel = _get_pixel_point_at(image, x, y);
    uint8_t *new_pixel = malloc(sizeof(uint8_t) * depth);
    switch(depth)
    {
        case 1:
            *new_pixel = *pixel;
            return new_pixel;
        case 2:
            new_pixel[0] = pixel[0];
            new_pixel[1] = pixel[1];
            return new_pixel;
        case 3:
            new_pixel[0] = pixel[0];
            new_pixel[1] = pixel[1];
            new_pixel[2] = pixel[2];
            return new_pixel;
        case 4:
            new_pixel[0] = pixel[0];
            new_pixel[1] = pixel[1];
            new_pixel[2] = pixel[2];
            new_pixel[3] = pixel[3];
            return new_pixel;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported pixel depth.");
    }
error:
    if(new_pixel)
        free(new_pixel);
    return NULL;
}

uint8_t tga_set_pixel_at(TGAImage *image, uint16_t x, uint16_t y, uint8_t *pix)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    check(pix, TGA_ARG_ERR, "Pixel data is NULL.");

    uint8_t depth = (uint8_t)((tga_get_pixel_depth(image) + 7) / 8);
    uint64_t data_offset = (y * tga_get_width(image) * depth) + (x * depth);
    for(int i = 0; i < depth; i++)
    {
        image->data[data_offset + i] = pix[i];
    }
    return 1;
error:
    return 0;
}

uint8_t tga_set_pixel_block(TGAImage *image, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height, uint8_t *pixel)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(_coordinate_sanity(image, x, y), tga_error(), tga_error_str());
    check(pixel, TGA_ARG_ERR, "Pixel data is NULL.");

    uint8_t depth = (uint8_t)((tga_get_pixel_depth(image) + 7) / 8);
    uint64_t line_offset = 0;
    for(uint16_t line = y; line < height+y; line++)
    {
        line_offset = (line*tga_get_width(image)*depth);
        for(uint16_t loc = x; loc < width+x; loc++)
        {
            memcpy(image->data + line_offset + (loc * depth), pixel, depth);
        }
    }
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
