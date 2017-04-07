#include <Image.h>
#include <TGAImage.h>
#include <stdio.h>

void print_stuff(TGAImage *img);
void print_stuff(TGAImage *img)
{
    printf("TGA VERSION: %d\n", img->version);
    printf("TGA ID FIELD LEN: %d\n", tga_get_id_field_length(img));
    printf("TGA C_MAP TYPE: %d\n", tga_get_color_map_type(img));
    printf("TGA Has Color Map: %s\n", tga_has_color_map(img) ? "True":"False");
    printf("TGA Get image Type: %d\n", tga_get_image_type(img));
    printf("TGA Get Color Map Start: %d\n", tga_get_color_map_start(img));
    printf("TGA Get Color Map Length: %d\n", tga_get_color_map_length(img));
    printf("TGA Get Color Map Depth: %d\n", tga_get_color_map_depth(img));
    printf("TGA Get X Offset: %d\n", tga_get_x_offset(img));
    printf("TGA Get Y Offset: %d\n", tga_get_y_offset(img));
    printf("TGA Get Width: %d\n", tga_get_width(img));
    printf("TGA Get Height: %d\n", tga_get_height(img));
    printf("TGA Get Pixel Depth: %d\n", tga_get_pixel_depth(img));
    int x=-1,y=-1;
    tga_get_origin_coordinates(img, &x, &y);
    printf("TGA Get Origin Coords: %d:%d\n", x, y);
    printf("TGA Get Attribute Bits: %d\n", tga_get_attribute_bits(img));
    printf("TGA Get Extension Offset: %d\n", tga_get_extension_offset(img));
    printf("TGA Get Developer Offset: %d\n", tga_get_developer_offset(img));
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("USAGE: image_tester [image_file]\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    TGAImage *image = read_tga_image(file);
    print_stuff(image);
    free_tga_image(image);
    fclose(file);
    return 0;
}
