#include <stdio.h>

#include <TGAImage.h>
#include <malloc.h>

static void print_tga_data(TGAImage *img)
{
    int x = 0, y = 0;
    printf("TGA VERSION: %d\n", img->version);
    printf("WIDTH: %d, HEIGHT: %d, DEPTH: %d\n", tga_get_width(img),
            tga_get_height(img), tga_get_pixel_depth(img));
    printf("IMAGE TYPE: ");
    switch(tga_get_image_type(img))
    {
        case TGA_COLOR_MAPPED:
            printf("Color Mapped\n");
            break;
        case TGA_TRUECOLOR:
            printf("Truecolor\n");

            break;
        case TGA_MONOCHROME:
            printf("Monochrome\n");
            break;
        default:
            printf("Unsupported\n");
    }
    printf("C_MAP_START: %d\n", tga_get_color_map_start(img));
    printf("C_MAP_DEPTH: %d\n", tga_get_color_map_depth(img));
    printf("C_MAP_LENGTH: %d\n", tga_get_color_map_length(img));
    printf("Attribute Bits: %d\n", tga_get_attribute_bits(img));
    tga_get_origin_coordinates(img, &x, &y);
    printf("Origin Point: %d,%d\n", x, y);

    uint8_t *pixel = NULL;
    uint8_t depth = (uint8_t)((tga_get_pixel_depth(img) + 7) / 8);
    uint16_t width = tga_get_width(img);
    printf("FIRST 8 x 8 Pixel Values (Assuming 24-bit Pixels):\n"); 
    for(y = 0; y < 8; y++)
    {
        for(x = 0; x < 8; x++)
        {
            pixel = img->data + (y*width*depth) + (depth * x); 
            printf("#%02x%02x%02x ", pixel[2], pixel[1], pixel[0]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    static char *default_file = "TEST_TGA.TGA";
    if(argc < 2) {
        printf("No default file specified. Using default file: %s\n", default_file);
    } else if(argc > 2) {
        printf("Usage: TGAReader <optional file>\n");
        return 1;
    }

    TGAImage *img = new_tga_image(TGA_TRUECOLOR, 32, 128, 255);
    if(!img) {
        return tga_error();
    }
    for(int x = 0; x < tga_get_width(img); x++)
    {
	    for(int y = 0; y < tga_get_height(img); y++)
	    {
		    tga_set_alpha_at(img, x, y, 255);
		    tga_set_red_at(img, x, y, y);
		    tga_set_green_at(img, x, y, y);
		    tga_set_blue_at(img, x, y, y);
	    } 
    }

    char *file_path = argc == 2 ? argv[1] : default_file;
    write_tga_image(img, file_path);
    free_tga_image(img);
    return 0;
}
