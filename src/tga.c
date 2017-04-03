#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <ImageDefs.h>
#include <TGAImage.h>
#include <Util.h>

#define TGA_HEADER_SIZE 18
#define TGA_FOOTER_SIZE 26
#define __TGA_SIG_SIZE  18
#define TRUEVISION_SIG "TRUEVISION-XFILE."

/*
 * The following structure contains both the footer and header data of the TGA
 * image. This is for data packing purposes.
 */
struct _NY_TgaMeta {
    uint32_t extension_offset;
    uint32_t developer_offset;

    uint16_t c_map_length;
    uint16_t x_offset;
    uint16_t y_offset;
    uint16_t width;
    uint16_t height;
    uint16_t c_map_start;

    uint8_t id_length;
    uint8_t c_map_type;
    uint8_t image_type;
    uint8_t pixel_depth;
    uint8_t c_map_depth;
    uint8_t image_descriptor;
    char __padding[2];
}; /* SIZEOF == 28 */

/* Trivial Sanity Check function for functions expecting an allocated image */
static inline uint8_t tga_sanity(TGAImage* image)
{
    check(image, "TGAImage was Null.");
    check(image->_meta, "TGAImage lacked metadata.");
    return 1;
error:
    return 0;
}


/*
 * The TGA Footer is only present in version 2 of the TGA Specification.
 * This function should be called first to see if the file contains a valid TGA
 * Footer, and thus determine if this is a TGA V2 file, rather than a V1 file.
 */
static uint8_t _read_tga_footer(TGAImage *image, FILE *file)
{
    uint8_t footer_buffer[TGA_FOOTER_SIZE];
    uint32_t ext_off;
    uint32_t dev_off;
    if(!tga_sanity(image)) /*Typical sanity check*/
        goto error;

    check(fseek(file, -TGA_FOOTER_SIZE, SEEK_END) == 0,
            "Unable to seek to TGA Footer.");
    check(fread(&footer_buffer, TGA_FOOTER_SIZE, 1, file) == 1,
            "Unable to read TGA Footer from File.");

    /* If the footer contains the signature "TRUEVISION-XFILE.\0", then it is
     * a version 2 file. Otherwise, it is random data and is version 1. */
    if(strncmp(((char *)(footer_buffer + 8)),
            TRUEVISION_SIG, __TGA_SIG_SIZE-1) == 0)
    {
        image->version = 2;
        ext_off = footer_buffer[0];
        ext_off += ((uint16_t)(footer_buffer[1])) << 8;
        ext_off += ((uint32_t)(footer_buffer[2])) << 16;
        ext_off += ((uint32_t)(footer_buffer[3])) << 24;
        image->_meta->extension_offset = ext_off;

        dev_off = footer_buffer[4];
        dev_off += ((uint16_t)(footer_buffer[5])) << 8;
        dev_off += ((uint32_t)(footer_buffer[6])) << 16;
        dev_off += ((uint32_t)(footer_buffer[7])) << 24;
        image->_meta->developer_offset = dev_off;
    }
    else
    {
        image->version = 1;
        image->_meta->extension_offset = 0;
        image->_meta->developer_offset = 0;
    }

    return 1;

error:
    if(image)
    {
        if(image->_meta)
        {
            image->_meta->extension_offset = 0;
            image->_meta->developer_offset = 0;
        }
    }
    return 0;
}

/*
 * TGA Header Structure Follows the following pattern:
 *      0x00: (1 byte) IDLength
 *      0x01: (1 byte) ColorMapType
 *      0x02: (1 byte) ImageType
 *      0x03: (2 byte) ColorMapStart
 *      0x05: (2 byte) ColorMapLength
 *      0x07: (1 byte) ColorMapDepth
 *      0x08: (2 byte) XOffset
 *      0x0A: (2 byte) YOffset
 *      0x0C: (2 byte) Width
 *      0x0E: (2 byte) Height
 *      0x10: (1 byte) PixelDepth
 *      0x11: (1 byte) ImageDescriptor
 *  Total Size: 18 Bytes
 */
static int _read_tga_header(TGAImage *image, FILE *file)
{
    uint8_t data[TGA_HEADER_SIZE];

    /* Sanity Checks */
    if(!tga_sanity(image))
        goto error;
    check(file, "Invalid File Pointer Passed.");
    /* Ensure that we are at the beginning of the file. */
    check(fseek(file, 0, SEEK_SET) == 0,
            "Unable to seek to beginning of file.");

    check(fread(&data, TGA_HEADER_SIZE, 1, file) == 1, "Unable to read file.");
    image->_meta->id_length = data[0];
    image->_meta->c_map_type = data[1];
    image->_meta->image_type = data[2];
    image->_meta->c_map_start = *((uint16_t*)(data+3));
    image->_meta->c_map_length = *((uint16_t*)(data+5));
    image->_meta->c_map_depth = data[7];
    image->_meta->x_offset = *((uint16_t*)(data+8));
    image->_meta->y_offset = *((uint16_t*)(data+10));
    image->_meta->width = *((uint16_t*)(data+12));
    image->_meta->height = *((uint16_t*)(data+14));
    image->_meta->pixel_depth = data[16];
    image->_meta->image_descriptor = data[17];
    return 1;

error:
    return 0;
}

static int _read_tga_id_field(TGAImage *image, FILE *file)
{
    check(file, "Invalid File.");
    check(image, "Invalid Image Pointer.");
    check(image->_meta->id_length != 0,
            "TGA Image ID Length is 0. This should not have been called.");

    image->id_field = malloc(image->_meta->id_length);
    check(image->id_field, "Unable to allocate memory for TGA ID Field.");

    if(ftell(file) != 18) /*Unlikely, assuming no problems, but never assume.*/
        check(fseek(file, 18, SEEK_SET) == 0, "Unable to seek to ID Field.");

    check(fread(image->id_field, image->_meta->id_length, 1, file) == 1,
            "Unable to read ID Field from file.");
    return 1;

error:
    if(image)
    {
        if(image->id_field)
        {
            free(image->id_field);
        }
        image->id_field = NULL;
    }
    return 0;
}

static int _read_tga_image_data(TGAImage *image, FILE *file)
{
    if(!tga_sanity(image))
        goto error;
    check(!image->data, "Image data exists already.");
    check(file, "Invalid File Pointer passed.");
    uint32_t offset = TGA_HEADER_SIZE + image->_meta->id_length +
            (image->_meta->c_map_length * (image->_meta->c_map_depth/8)) +
            image->_meta->c_map_start;
    if(ftell(file) != offset)
        fseek(file, offset, SEEK_SET);

    uint8_t depth_mult = (image->_meta->pixel_depth % 8 != 0) ?
            (image->_meta->pixel_depth/8)+1 : image->_meta->pixel_depth/8;
    uint32_t total = image->_meta->width * image->_meta->height * depth_mult;
    image->data = malloc(sizeof(uint8_t) * total);
    check(image->data, "Unable to allocate image data.");
    check(fread(image->data, total, 1, file) == 1,
            "Unable to read image pixel data.");

    return 1;

error:
    if(image->data)
        free(image->data);
    image->data = NULL;
    return 0;
}

static int _allocate_tga_data(TGAImage *image, ChannelType ct,
        uint16_t width, uint16_t height)
{
    if(!tga_sanity(image))
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

TGAImage *read_tga_image(FILE *file)
{
    check(file, "Invalid file passed.");

    TGAImage *image = new_tga_image(CT_NONE, 0, 0);
    check(image, "Unable to create new TGAImage.");
    check(_read_tga_footer(image, file), "Unable to read TGA Footer.");
    check(_read_tga_header(image, file), "Unable to read TGA Header.");

    if(image->_meta->id_length != 0)
        check(_read_tga_id_field(image, file), "Unable to read TGA ID Field.");
    else
        image->id_field = NULL;

    if(image->_meta->c_map_type == TGA_COLOR_MAPPED ||
        image->_meta->c_map_type == TGA_ENCODED_COLOR_MAPPED)
        sentinel("Unfortunately, ColorMapped TGA Files are not yet supported.");

    check(_read_tga_image_data(image, file), "Unable to read TGA Image Data.");
    return image;

error:
    if(image)
    {
        free(image);
    }
    return NULL;
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

uint8_t tga_get_id_field_length(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->id_length;
    return 0;
}

uint8_t tga_has_color_map(TGAImage *image)
{
    return tga_get_color_map_type(image);
}

uint8_t tga_get_color_map_type(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->c_map_type;
    return 0;
}

TGAColorType tga_get_image_type(TGAImage *image)
{
    if(!tga_sanity(image))
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
    if(tga_sanity(image))
        return image->_meta->c_map_start;
    return 0;
}

uint16_t tga_get_color_map_length(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->c_map_length;
    return 0;
}

uint8_t tga_get_color_map_depth(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->c_map_depth;
    return 0;
}

uint16_t tga_get_x_offset(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->x_offset;
    return 0;
}

uint16_t tga_get_y_offset(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->y_offset;
    return 0;
}

uint16_t tga_get_width(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->width;
    return 0;
}

uint16_t tga_get_height(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->height;
    return 0;
}

uint8_t tga_get_pixel_depth(TGAImage *image)
{
    if(tga_sanity(image))
        return image->_meta->pixel_depth;
    return 0;
}

uint32_t tga_get_extension_offset(TGAImage *img)
{
    if(!tga_sanity(img) || img->version == 1)
        return 0;
    return img->_meta->extension_offset;
}

uint32_t tga_get_developer_offset(TGAImage *img)
{
    if(!tga_sanity(img) || img->version == 1)
        return 0;
    return img->_meta->developer_offset;
}

void tga_get_origin_coordinates(TGAImage *image, int *x, int *y)
{
    if(tga_sanity(image))
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
    if(!tga_sanity(image))
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
    check(tga_sanity(image), "Problem getting red channel.");
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
    check(tga_sanity(image), "Problem getting green channel.");
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
    check(tga_sanity(image), "Problem getting blue channel.");
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
