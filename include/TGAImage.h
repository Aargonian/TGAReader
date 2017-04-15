#ifndef __NY_TGA_FILE
#define __NY_TGA_FILE

#include <stdio.h>
#include <stdint.h>

typedef enum {
    TGA_NO_ERR                  = 0,
    TGA_INV_IMAGE_PNT           = 1,
    TGA_INV_FILE_PNT            = 2,
    TGA_MEM_ERR                 = 3,
    TGA_GEN_IO_ERR              = 4,
    TGA_READ_ERR                = 5,
    TGA_WRITE_ERR               = 6,
    TGA_IMAGE_IMMUTABLE_ERR     = 7,
    TGA_UNSUPPORTED             = 8,
    TGA_INTERNAL_ERR            = 9
} TGAError;

typedef enum {
    TGA_NO_DATA                 = 0,
    TGA_COLOR_MAPPED            = 1,
    TGA_TRUECOLOR               = 2,
    TGA_MONOCHROME              = 3,
    TGA_ENCODED_COLOR_MAPPED    = 9,
    TGA_ENCODED_TRUECOLOR       = 10,
    TGA_ENCODED_MONOCHROME      = 11,
    TGA_UNKNOWN_TYPE            = 255
} TGAColorType;

struct _NY_TgaMeta;

typedef struct NyTGA_Image {
    uint8_t *id_field;
    uint8_t *data;
    struct _NY_TgaMeta *_meta;
    uint8_t version;
    char __padding[7];
} TGAImage; /* SIZEOF == 24 */

TGAError TGA_ERR;

TGAError tga_error(void); /* Returns the current error, if any. */
void tga_clear_error(void);
TGAImage *read_tga_image(FILE *file);
TGAImage *new_tga_image(TGAColorType type, uint8_t depth,
                        uint16_t width, uint16_t height);
void free_tga_image(TGAImage* image);

/* Getters */
uint8_t tga_get_id_field_length(TGAImage *image);
uint8_t tga_get_color_map_type(TGAImage *image);
uint8_t tga_has_color_map(TGAImage *image);
TGAColorType tga_get_image_type(TGAImage *image);
uint16_t tga_get_color_map_start(TGAImage *image);
uint16_t tga_get_color_map_length(TGAImage *image);
uint8_t tga_get_color_map_depth(TGAImage *image);
uint16_t tga_get_x_offset(TGAImage *image);
uint16_t tga_get_y_offset(TGAImage *image);
uint16_t tga_get_width(TGAImage *image);
uint16_t tga_get_height(TGAImage *image);
uint8_t tga_get_pixel_depth(TGAImage *image);
void tga_get_origin_coordinates(TGAImage * image, int *x, int *y);
uint8_t tga_get_attribute_bits(TGAImage *image);

uint32_t tga_get_extension_offset(TGAImage *image);
uint32_t tga_get_developer_offset(TGAImage *image);

uint8_t tga_get_red_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_green_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_blue_at(TGAImage *image, uint16_t x, uint16_t y);

#endif/*__NY_TGA_FILE*/
