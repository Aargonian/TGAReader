#include <stdio.h>

#include <TGAImage.h>

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
    uint8_t depth = (tga_get_pixel_depth(img)+7) / 8;
    uint8_t width = tga_get_width(img);
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
    if(argc != 2)
        return 1;

    TGAImage *img = new_tga_image(TGA_TRUECOLOR, 32, 128, 255);
    if(!img) {
        printf("TGA ERROR: %s", tga_error_str());
        return tga_error();
    }
    uint8_t *red = tga_create_pixel_for_image(img, 255, 0, 0, 255);
    uint8_t *green = tga_create_pixel_for_image(img, 0, 255, 0, 255);
    uint8_t *blue = tga_create_pixel_for_image(img, 0, 0, 255, 255);
    uint8_t *white = tga_create_pixel_for_image(img, 255, 255, 255, 255);
    /*tga_set_pixel_block(img, 0, 0, 32, 128, red);
    tga_set_pixel_block(img, 32, 0, 32, 128, green);
    tga_set_pixel_block(img, 64, 0, 32, 128, blue);
    tga_set_pixel_block(img, 96, 0, 32, 128, white);*/
    /*uint8_t *blue = tga_create_pixel_for_image(img, 255, 0, 0, 255);
    tga_set_pixel_block(img, 0, 0, 128, 128, blue);*/
    //print_tga_data(img);
    for(int x = 0; x < tga_get_width(img); x++)
    {
	    for(int y = 0; y < tga_get_height(img); y++)
	    {
		    tga_set_alpha_at(img, x, y, 255);
		    tga_set_red_at(img, x, y, y);
		    tga_set_green_at(img, x, y, y);
		    tga_set_blue_at(img, x, y, y < 255 ? y : 255);
	    } 
    }
    write_tga_image(img, argv[1]);
    free_tga_image(img);
    return 0;
}
