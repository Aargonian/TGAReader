#include <stdint.h>
#include <stdlib.h>

#include "Private/TGAPrivate.h"

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
    if(!_tga_sanity(image))
        goto error;

    check(fseek(file, -TGA_FOOTER_SIZE, SEEK_END) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to TGA Footer.");
    check(fread(&footer_buffer, TGA_FOOTER_SIZE, 1, file) == 1, TGA_READ_ERR,
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
    uint8_t data[TGA_HEADER_SIZE] = {0};

    /* Sanity Checks */
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(file, TGA_INV_FILE_PNT, "Invalid File Pointer Passed.");
    /* Ensure that we are at the beginning of the file. */
    check(fseek(file, 0, SEEK_SET) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to beginning of file.");

    check(fread(&data, TGA_HEADER_SIZE, 1, file) == 1, TGA_READ_ERR,
            "Unable to read file.");
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
    check(file, TGA_INV_FILE_PNT, "Invalid File.");
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(image->_meta->id_length != 0, TGA_INTERNAL_ERR,
            "TGA Image ID Length is 0. This should not have been called.");

    image->id_field = malloc(image->_meta->id_length);
    check(image->id_field, TGA_MEM_ERR,
            "Unable to allocate memory for TGA ID Field.");

    if(ftell(file) != TGA_HEADER_SIZE)
        check(fseek(file, TGA_HEADER_SIZE, SEEK_SET) == 0, TGA_GEN_IO_ERR,
                "Unable to seek to ID Field.");

    check(fread(image->id_field, image->_meta->id_length, 1, file) == 1,
            TGA_READ_ERR, "Unable to read ID Field from file.");
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

static int _read_tga_color_map(TGAImage *image, FILE *file)
{
    uint16_t c_map_size = 0;
    uint16_t start = 0;
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(file, TGA_INV_FILE_PNT, "Invalid File Pointer.");
    check(image->_meta->image_type == TGA_COLOR_MAPPED, TGA_INTERNAL_ERR,
            "Image is not color-mapped. This should not have been called.");
    c_map_size = ((image->_meta->c_map_depth+7)/8) * image->_meta->c_map_length;
    check(c_map_size > 0, TGA_COLOR_MAP_ERR, "Image claims color map, but "
            "map is of size 0.");

    start = TGA_HEADER_SIZE + image->_meta->id_length +
            image->_meta->c_map_start;
    check(fseek(file, start, SEEK_SET) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to color map start.");

    image->color_map = malloc(sizeof(uint8_t) * c_map_size);
    check(image->color_map, TGA_MEM_ERR, "Unable to allocate color map.");
    check(fread(image->color_map, c_map_size, 1, file) == 1, TGA_READ_ERR,
            "Unable to read Color Map.");
    return 1;
error:
    return 0;
}

/* TODO: Implement Reading Color-Mapped Encoded Images. */
static int _read_encoded_tga_image_data(TGAImage *image, FILE *file)
{
    uint16_t line = 0;
    uint32_t offset = (uint32_t)TGA_HEADER_SIZE + image->_meta->id_length +
            (image->_meta->c_map_length * (image->_meta->c_map_depth/8)) +
            image->_meta->c_map_start;
    uint8_t depth = (uint8_t)((image->_meta->pixel_depth+7)/8);
    uint32_t total = image->_meta->width * image->_meta->height * depth;
    uint8_t packet = 0;
    uint8_t current_packet_cnt = 0;
    uint16_t current_line_pos = 0;
    uint8_t current_pixel = 0;
    uint32_t data_offset = 0;
    uint8_t *run_packet = malloc(sizeof(uint8_t)*depth);

    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    check(file, TGA_INV_FILE_PNT, "Invalid File Pointer.");
    check(image->_meta->image_type != TGA_ENCODED_COLOR_MAPPED, TGA_UNSUPPORTED,
            "Unfortunately, Encoded Color Map Images are not supported yet.");
    check(image->_meta->image_type == TGA_ENCODED_TRUECOLOR ||
            image->_meta->image_type == TGA_ENCODED_MONOCHROME,
            TGA_INTERNAL_ERR, "Not encoded image. Should not have been called");
    check(fseek(file, offset, SEEK_SET) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to data begining.");

    image->data = malloc(sizeof(uint8_t) * total);

    for(line = 0; line < image->_meta->height-1; line++)
    {
        current_line_pos = 0; /* In Pixels, Not Bytes */
        while(current_line_pos < image->_meta->width)
        {
            check(fread(&packet, sizeof(packet), 1, file) == 1,
                    TGA_READ_ERR, "Failed to read packet");
            current_packet_cnt = (packet & 127) + 1; /* Also in Pixels */

            /* Offset from beginning, in bytes, hence multiply by depth */
            data_offset = (current_line_pos + (line*image->_meta->width))*depth;

            if(packet & 128) /*X & 10000000b */
            {
                /* Run-length packet, copy the following value count times */
                check(fread(run_packet, depth, 1, file) == 1, TGA_READ_ERR,
                        "Failed to read RLE pixel packet.");
                for(current_pixel = 0; current_pixel < current_packet_cnt; current_pixel++)
                    memcpy(image->data+data_offset+(depth*current_pixel),
                            run_packet, depth);
            }
            else
            {
                /* Raw-packet, read the following cnt values */
                check(fread(image->data+data_offset, depth,
                            current_packet_cnt, file) == current_packet_cnt,
                        TGA_READ_ERR, "Unable to read Raw Pixel Values.");
            }
            current_line_pos += current_packet_cnt;
        }
    }
    return 1;
error:
    if(image->data)
        free(image->data);
    return 0;
}

static int _read_unencoded_tga_image_data(TGAImage *image, FILE *file)
{
    check(_tga_sanity(image), TGA_INV_IMAGE_PNT, "Invalid TGAImage Pointer.");
    if(image->data)
        free(image->data);
    check(file, TGA_INV_FILE_PNT, "Invalid File Pointer passed.");
    int32_t offset = (uint32_t)TGA_HEADER_SIZE + image->_meta->id_length +
            (image->_meta->c_map_length * (image->_meta->c_map_depth/8)) +
            image->_meta->c_map_start;
    check(fseek(file, offset, SEEK_SET) == 0, TGA_GEN_IO_ERR,
            "Unable to seek to data offset.");

    uint8_t depth_mult = (uint8_t)((image->_meta->pixel_depth+7) / 8);
    uint32_t pixels = image->_meta->width * image->_meta->height;
    image->data = malloc(sizeof(uint8_t) * pixels * depth_mult);
    check(image->data, TGA_MEM_ERR, "Unable to allocate image data.");
    check(fread(image->data, pixels * depth_mult, 1, file) == 1, TGA_READ_ERR,
            "Unable to read image pixel data.");

    return 1;

error:
    if(image->data)
        free(image->data);
    image->data = NULL;
    return 0;
}

/* Most errors in this subroutine are already set by the lower-level functions.
 * So the error is set using tga_error() to fetch the existing error. */
/* TODO: Implement reading for Encoded TGA Images. */
/* TODO: Implement reading for developer/extension areas. */
TGAImage *read_tga_image(FILE *file)
{
    TGAImage *image = NULL;

    check(file, TGA_INV_FILE_PNT, "Invalid file passed.");

    image = new_tga_image(TGA_NO_DATA, 0, 0, 0);
    check(image, tga_error(), "Unable to create new TGAImage.");
    check(_read_tga_footer(image, file), tga_error(),
            "Unable to read TGA Footer.");
    check(_read_tga_header(image, file), tga_error(),
            "Unable to read TGA Header.");

    if(image->_meta->id_length)
        check(_read_tga_id_field(image, file), tga_error(),
                "Unable to read TGA ID Field.");
    else
        image->id_field = NULL;

    if(image->_meta->image_type == TGA_COLOR_MAPPED)
        check(_read_tga_color_map(image, file), tga_error(),
            "Unable to read TGA ColorMap Data.");

    switch(image->_meta->image_type)
    {
        case TGA_ENCODED_TRUECOLOR:
            check(_read_encoded_tga_image_data(image, file), tga_error(),
                    "Unable to read Encoded Truecolor TGA Image Data.");
            image->_meta->image_type = TGA_TRUECOLOR;
            break;
        case TGA_ENCODED_MONOCHROME:
            check(_read_encoded_tga_image_data(image, file), tga_error(),
                    "Unable to read Encoded Monochrome TGA Image Data.");
            image->_meta->image_type = TGA_MONOCHROME;
            break;
        case TGA_TRUECOLOR:
        case TGA_MONOCHROME:
        case TGA_COLOR_MAPPED:
            check(_read_unencoded_tga_image_data(image, file), tga_error(),
                    "Unable to read TGA Image Data.");
            break;
        default:
            fail(TGA_UNSUPPORTED, "Unsupported TGA Format.");
    }

    return image;

error:
    if(image)
    {
        free(image);
    }
    return NULL;
}
