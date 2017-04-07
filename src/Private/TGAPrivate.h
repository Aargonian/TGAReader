#include <stdint.h>
#include <stdbool.h>
#include <TGAImage.h>
#include <Util.h>

#define TGA_HEADER_SIZE 18
#define TGA_FOOTER_SIZE 26
#define __TGA_SIG_SIZE  18
#define TRUEVISION_SIG "TRUEVISION-XFILE."

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
    check(image, "TGAImage was Null.");
    check(image->_meta, "TGAImage lacked metadata.");
    return true;
error:
    return false;
}
