#ifndef _TGA_PRIVATE_H
#define _TGA_PRIVATE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <memory.h>

#include <TGAImage.h>

#define TGA_HEADER_SIZE 18
#define TGA_FOOTER_SIZE 26
#define __TGA_SIG_SIZE  18
#define TGA_ERR_MAX     256
#define TRUEVISION_SIG "TRUEVISION-XFILE."

extern TGAError tga_err;
extern char tga_err_string[TGA_ERR_MAX];

/*
 * The below macros are used to perform a check on a simple boolean condition.
 * If the condition is false, assume an error, set the specified error flag, log
 * the error if debugging has been enabled, set the error string, and goto the
 * error section. I've done this because I believe it keeps the code easier to
 * read/modify due to the numerous places where all the above would have to be
 * repeated anyway without the macros.
 */
#define errno_str() (errno == 0 ? "NO ERRNO ERROR" : strerror(errno))
#define __ERR_TEXT "[ERROR] (%s:%d - errno: %s) "

#ifdef _TGA_DEBUG
    #define do_print_err(...)                                           \
        fprintf(stderr, __ERR_TEXT, __FILE__, __LINE__, errno_str());   \
        fprintf(stderr, __VA_ARGS__);                                   \
        fprintf(stderr, "\n");
#else
    #define do_print_err(...)
#endif/*_TGA_DEBUG*/

#define check(A, _NY_ERR, ...)                                  \
    do {                                                        \
        if(!(A)) {                                              \
            tga_err = _NY_ERR;                                  \
            do_print_err(__VA_ARGS__);                          \
            snprintf(tga_err_string, TGA_ERR_MAX, __VA_ARGS__); \
            goto error;                                         \
        }                                                       \
    } while(0)

/* The below macro is for unimplemented functions, or 'impossible' branches. */
#define fail(_TGA_ERR, ...) check(false, _TGA_ERR, __VA_ARGS__)

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
static bool _tga_sanity(TGAImage* image)
{
    return image && image->_meta;
}

#endif/*_TGA_PRIVATE_H*/
