#include <stdint.h>
#include <stdlib.h>

#include "Private/TGAPrivate.h"

static int _write_tga_header(TGAImage *image, FILE *file)
{
    uint8_t data[TGA_HEADER_SIZE] = {0};
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(file, TGA_INV_FILE_PNT, "Invalid file pointer passed.");
    check(fseek(file, 0, SEEK_SET) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to beginning of file.");
    data[0] = image->_meta->id_length;
    data[1] = image->_meta->c_map_type;
    data[2] = image->_meta->image_type;
    *((uint16_t*)(data+3)) = image->_meta->c_map_start;
    *((uint16_t*)(data+5)) = image->_meta->c_map_length;
    data[7] = image->_meta->c_map_depth;
    *((uint16_t*)(data+8)) = image->_meta->x_offset;
    *((uint16_t*)(data+10)) = image->_meta->y_offset;
    *((uint16_t*)(data+12)) = image->_meta->width;
    *((uint16_t*)(data+14)) = image->_meta->height;
    data[16] = image->_meta->pixel_depth;
    data[17] = image->_meta->image_descriptor;

    check(fwrite(data, TGA_HEADER_SIZE, 1, file) == 1, TGA_WRITE_ERR,
            "Unable to write TGA Header.");
    return 1;
error:
    return 0;
}

static int _write_tga_id_field(TGAImage *image, FILE *file)
{
    check(file, TGA_INV_FILE_PNT, "Invalid FILE Pointer.");
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(image->_meta->id_length != 0 && image->id_field, TGA_INTERNAL_ERR,
            "TGA Image ID Null or 0. This should not have been called.");
    if(ftell(file) != TGA_HEADER_SIZE)
        check(fseek(file, TGA_HEADER_SIZE, SEEK_SET) == 0, TGA_GEN_IO_ERR,
                "Unable to seek to beginning of ID Field.");
    check(fwrite(image->id_field, image->_meta->id_length, 1, file) == 1,
            TGA_WRITE_ERR, "Unable to write ID Field to file.");
    return 1;
error:
    return 0;
}

static int _write_tga_image_data(TGAImage *image, FILE *file)
{
    uint8_t depth = 0;
    uint32_t total = 0;

    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(file, TGA_INV_FILE_PNT, "Invalid File Pointer.");

    if(image->_meta->image_type == TGA_NO_DATA ||
        image->_meta->image_type == TGA_INVALID_TYPE)
        return 1; /* No Data to be written, according to meta-data. */

    depth = (uint8_t)((image->_meta->pixel_depth + 7) / 8);
    total = image->_meta->width * image->_meta->height * depth;
    if(total == 0)
        return 1;

    check(image->data, TGA_INV_IMAGE_PNT, "Data missing.");
    check(fwrite(image->data, total, 1, file) == 1, TGA_WRITE_ERR,
            "Unable to write image data to file.");
    return 1;
error:
    return 0;
}

int write_tga_image(TGAImage *image, const char* filename)
{
    FILE *file = NULL;
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(image->_meta->image_type == TGA_TRUECOLOR, TGA_UNSUPPORTED,
            "Currently, only writing for TRUECOLOR TGA Images is supported.");
    check(image->version == 1 || image->version == 2, TGA_UNSUPPORTED,
            "Unsupported TGA Version.");
    check(filename && filename[0] != '\0', TGA_INV_FILE_NAME,
            "Invalid or Null filename.");
    file = fopen(filename, "wb");
    check(file, TGA_WRITE_ERR, "Unable to open file for writing.");

    check(_write_tga_header(image, file), tga_error(),
            "Unable to write TGA Header.");
    if(image->_meta->id_length > 0)
        check(_write_tga_id_field(image, file), tga_error(),
                "Unable to write TGA ID Field.");
    check(_write_tga_image_data(image, file), tga_error(),
            "Unable to write TGA Data to file.");
    return 1;
error:
    if(file)
        fclose(file);
    return 0;
}
