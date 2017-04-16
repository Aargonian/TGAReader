#include <stdio.h>

#include <TGAImage.h>

static void print_tga_data(TGAImage *img)
{
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
}

int main(int argc, char **argv)
{
    if(argc != 3)
        return 1;

    FILE *img_file = fopen(argv[1], "rb");
    if(!img_file)
        return tga_error();
    TGAImage *img = read_tga_image(img_file);
    if(!img)
        return tga_error();
    print_tga_data(img);
    write_tga_image(img, argv[2]);
    free_tga_image(img);
    fclose(img_file);
    return 0;
}
