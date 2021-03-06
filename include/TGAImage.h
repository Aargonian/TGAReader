#ifndef __NY_TGA_FILE
#define __NY_TGA_FILE

#include <stdio.h>
#include <stdint.h>

typedef enum {
    TGA_NO_ERR                  = 0,
    TGA_INV_IMAGE_PNT           = 1,
    TGA_INV_FILE_PNT            = 2,
    TGA_COLOR_MAP_ERR           = 3,
    TGA_MEM_ERR                 = 4,
    TGA_GEN_IO_ERR              = 5,
    TGA_READ_ERR                = 6,
    TGA_WRITE_ERR               = 7,
    TGA_INV_FILE_NAME           = 8,
    TGA_UNSUPPORTED             = 10,
    TGA_TYPE_ERR                = 11,
    TGA_ARG_ERR                 = 12,
    TGA_INTERNAL_ERR            = 255
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
    uint8_t *color_map;
    struct _NY_TgaMeta *_meta;
    uint8_t version;
    char __padding[7];
} TGAImage; /* SIZEOF == 24 */

TGAError tga_error(void); /* Returns the current error, if any. */
char *tga_error_str(void); /* Returns a string with error details. */
void tga_clear_error(void);
TGAImage *read_tga_image(FILE *file);
int write_tga_image(TGAImage *image, const char *filename);
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
uint8_t tga_is_monochrome(TGAImage *image);

uint32_t tga_get_extension_offset(TGAImage *image);
uint32_t tga_get_developer_offset(TGAImage *image);

uint8_t tga_get_red_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_green_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_blue_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_alpha_at(TGAImage *image, uint16_t x, uint16_t y);
uint8_t tga_get_mono_at(TGAImage *image, uint16_t x, uint16_t y);

uint8_t tga_set_red_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t red);
uint8_t tga_set_green_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t green);
uint8_t tga_set_blue_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t blue);
uint8_t tga_set_alpha_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t alpha);
uint8_t tga_set_mono_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t value);

uint8_t *tga_create_pixel_for_image(TGAImage* img,
                                    uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void tga_free_pixel(uint8_t* pixel);

uint8_t *tga_get_pixel_copy_at(TGAImage *img, uint16_t x, uint16_t y);
uint8_t tga_set_pixel_at(TGAImage *img, uint16_t x, uint16_t y, uint8_t *pixel);
uint8_t tga_set_pixel_block(TGAImage *image, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height, uint8_t *pixel);
#endif/*__NY_TGA_FILE*/
