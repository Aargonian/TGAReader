#include <stdio.h>

#include <TGAImage.h>

static void print_tga_data(TGAImage *img)
{
    printf("TGA VERSION: %d\n", img->version);
    printf("WIDTH: %d, HEIGHT: %d, DEPTH: %d\n", tga_get_width(img),
            tga_get_height(img), tga_get_pixel_depth(img));
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "USAGE: TGAReader <image>\n");
        return 1;
    }

    FILE *img_file = fopen(argv[1], "rb");
    if(!img_file)
    {
        fprintf(stderr, "Unable to open image file.\n");
        return tga_error();
    }
    TGAImage *img = read_tga_image(img_file);
    if(!img)
    {
        fprintf(stderr, "Unable to read image.");
        return tga_error();
    }
    print_tga_data(img);
    free_tga_image(img);
    fclose(img_file);
    return 0;
}
