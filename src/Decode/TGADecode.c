#include <stdlib.h>
#include <stdint.h>

#include "../Private/TGAPrivate.h"

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
    if(!_tga_sanity(image)) /*Typical sanity check*/
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
    if(!_tga_sanity(image))
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
    if(!_tga_sanity(image))
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
