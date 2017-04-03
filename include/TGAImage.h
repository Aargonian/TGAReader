#ifndef __NY_TGA_FILE
#define __NY_TGA_FILE

#include <stdio.h>
#include <stdint.h>

#include <Image.h>

typedef enum _NY_TGA_COLOR_TYPE {
    TGA_NO_DATA = 0,
    TGA_COLOR_MAPPED = 1,
    TGA_TRUECOLOR = 2,
    TGA_MONOCHROME = 3,
    TGA_ENCODED_COLOR_MAPPED = 9,
    TGA_ENCODED_TRUECOLOR = 10,
    TGA_ENCODED_MONOCHROME = 11,
    TGA_INVALID_TYPE = 255
} TGAColorType;

struct _NY_TgaMeta;

typedef struct NyTGA_Image {
    uint8_t *id_field;
    uint8_t *data;
    struct _NY_TgaMeta *_meta;
    uint8_t version;
    char __padding[7];
} TGAImage; /* SIZEOF == 24 */

TGAImage *read_tga_image(FILE *file);
TGAImage *new_tga_image(ChannelType type, uint16_t width, uint16_t height);
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

/* Setters */
Image *tga_to_generic(TGAImage *img);

/*
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
    char padding[2];
}; SIZEOF == 28
*/

#endif/*__NY_TGA_FILE*/
