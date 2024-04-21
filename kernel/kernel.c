#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "imageHex.h"
#include "image_info.h"
#include "kernel.h"

#define SIDE 16

static volatile int w, h; // Screen size

#define KEYNAME(key) \
    [AM_KEY_##key] = #key,
static const char *key_names[] = {AM_KEYS(KEYNAME)};

static inline void puts(const char *s)
{
    for (; *s; s++)
        putch(*s);
}

void print_key()
{
    AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
    ioe_read(AM_INPUT_KEYBRD, &event);
    if (event.keycode != AM_KEY_NONE && event.keydown)
    {
        puts("Key pressed: ");
        puts(key_names[event.keycode]);
        puts("\n");
    }
}

static void draw_tile(int x, int y, int w, int h, uint32_t color)
{
    uint32_t tile_pixels[w * h]; // WARNING: large stack-allocated memory
    AM_GPU_FBDRAW_T event = {
        .x = x,
        .y = y,
        .w = w,
        .h = h,
        .sync = 1,
        .pixels = tile_pixels,
    };
    for (int i = 0; i < w * h; i++)
    {
        tile_pixels[i] = color;
    }
    ioe_write(AM_GPU_FBDRAW, &event);
}

void splash()
{
    AM_GPU_CONFIG_T info = {0};
    ioe_read(AM_GPU_CONFIG, &info);
    w = info.width;
    h = info.height;

    for (int x = 0; x * SIDE <= w; x++)
    {
        for (int y = 0; y * SIDE <= h; y++)
        {
            // if ((x & 1) ^ (y & 1))
            // {
                draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xfff46f);
            // }
        }
    }
}

void draw_image()
{
    AM_GPU_CONFIG_T info = {0};
    ioe_read(AM_GPU_CONFIG, &info);
    w = info.width;
    h = info.height;
    printf("screen width :  %d \n", w);
    printf("screen height: %d\n", h);
    image_info img_info = parse_image(target_img);
    printf("image width: %d\n", img_info.width);
    printf("image height: %d\n", img_info.height);
    printf("The image color type is: %d\n", img_info.color_depth);
    volatile uint32_t pixel = 0xfff46f;
    println("target size: %d",sizeof(target_img));

    // scale image based on screen size
    printf("w: %d, h: %d, %d\n", w, h, w * h * 4 / SIDE / SIDE);
    unsigned char pixels[w*h*4/SIDE/SIDE]; 
    if (img_info.color_depth==32){
        nearestNeighborInterpolation(&(target_img[PNG_DATA_INDEX]), img_info.width, img_info.height, pixels, w/SIDE, h/SIDE);
    }else{
        nearestNeighborInterpolation_RGB(&(target_img[BMP_DATA_INDEX]), img_info.width, img_info.height, pixels, w/SIDE, h/SIDE); 
    }

    for (int x = 0; x * SIDE <= w; x++)
    {
        // println("pixel: %d",pixel);
        for (int y = 0; y * SIDE <= h; y++)
        {
            if (img_info.color_depth == 32) // 4 bytes for a pixel
            {
                int init_index = (y * w + x) * 4;
                pixel = (pixels[init_index] << 16) | (pixels[init_index + 1] << 8) |
                        pixels[init_index + 2];
            }
            else if (img_info.color_depth==24) // 3 bytes for a pixel
            {
                int init_index = BMP_DATA_INDEX + ((h/SIDE-y-1) * w/SIDE + x) * 3;
                println("index: %d",init_index);
                // pixel = (pixels[init_index] << 16) | (pixels[init_index + 1] << 8) |
                //         (pixels[init_index + 2]);                
                pixel = (pixels[init_index +2] << 16) | (pixels[init_index + 1] << 8) |
                        (pixels[init_index]);
            }
            // printf("pixel: %d, upper limit %d\n", pixel,0xffffff);
            draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, pixel);
        }
    }
}

// Operating system is a C program!
int main(const char *args)
{
    ioe_init();

    puts("mainargs = \"");
    puts(args); // make run mainargs=xxx
    puts("\"\n");

    //  splash();
    draw_image();
    puts("Press any key to see its key code...\n");
    while (1)
    {
        print_key();
    }
    return 0;
}
