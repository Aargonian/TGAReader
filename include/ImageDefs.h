#ifndef __NY_IMAGE_DEFS
#define __NY_IMAGE_DEFS

#include <stdint.h>

typedef enum {
    CT_NONE,
    CT_UNKNOWN,
    CT_CUSTOM,

    /* Unless otherwise stated, channels assume 8-bit */
    CT_MONOCHROME,
    CT_RGBA,
    CT_RGBA64, /* 16-bit */
    CT_RGB,
    CT_BGR,
    CT_ABGR,
    CT_BGRA,
    CT_ARGB, 

    /* 5-bits per channel, except 1-bit alpha */
    CT_R5G5B5A, /*RGBA*/
    CT_AR5G5B5, /*ARGB*/
    CT_B5G5R5A, /*BGRA*/
    CT_AB5G5R5, /*ABGR*/
} ChannelType;

typedef enum _NY_ImageFormat {
    FORMAT_TGA = 0,
    FORMAT_PNG = 1,
    FORMAT_GIF = 2,

    FORMAT_JPG = 3,
    FORMAT_JPEG = 3,

    FORMAT_BMP = 4,
    FORMAT_TIFF = 5,
    FORMAT_EXIF = 6,
    FORMAT_WEBP = 7,

    FORMAT_PNM = 8,
    FORMAT_PPM = 8,
    FORMAT_PGM = 8,
    FORMAT_PBM = 8,

    FORMAT_HDR = 9,
    FORMAT_HEIF = 10,
    FORMAT_BAT = 11,
    FORMAT_BPG = 12
} ImageFormat;

typedef struct __NY_IMAGE {
    uint16_t width;
    uint16_t height;
    ChannelType channel_type;

    uint8_t *data;
} Image;
#endif/*__NY_IMAGE_DEFS*/
