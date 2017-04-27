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

    printf("MAKING IMAGE\n");
    TGAImage *img = new_tga_image(TGA_TRUECOLOR, 32, 128, 128);
    if(!img) {
        printf("TGA ERROR: %s", tga_error_str());
        return tga_error();
    }
    printf("ENDING HERE.\n");
    uint8_t *red = tga_create_pixel_for_image(img, 255, 0, 0, 255);
    uint8_t *green = tga_create_pixel_for_image(img, 0, 255, 0, 255);
    uint8_t *blue = tga_create_pixel_for_image(img, 0, 0, 255, 255);
    uint8_t *white = tga_create_pixel_for_image(img, 255, 255, 255, 255);
    printf("MADE IT HERE.\n");
    tga_set_pixel_block(img, 0, 0, 64, 64, red);
    tga_set_pixel_block(img, 64, 0, 64, 64, green);
    tga_set_pixel_block(img, 0, 64, 64, 64, blue);
    tga_set_pixel_block(img, 64, 64, 64, 64, white);
    printf("BLOCKS DONE.\n");
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            tga_set_red_at(img, i, j, 255);
        }
    }
    print_tga_data(img);
    printf("FILE TO WRITE: %s\n", argv[1]);
    write_tga_image(img, argv[1]);
    printf("IMAGE TYPE TO WRITE: %d\n", tga_get_image_type(img));
    free_tga_image(img);
    return 0;
}
