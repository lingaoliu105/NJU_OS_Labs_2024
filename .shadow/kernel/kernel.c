#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>
#include "imageHex.h"
#include "image_info.h"
#include "mylib.h"
#include "kernel.h"
#define SIDE 16

static int w, h;  // Screen size

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = { AM_KEYS(KEYNAME) };

static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;


  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xff0000);
      }

    }
  }
}

void draw_image(){
    AM_GPU_CONFIG_T info = {0};
    ioe_read(AM_GPU_CONFIG, &info);
    w = info.width;
    h = info.height;
    my_printf("screen width :  %d \n",w);
    my_printf("screen height: %d\n",h);
    image_info img_info = parse_png_image(target_img);
    uint32_t pixel;
    for (int x = 0; x * SIDE <= w; x ++) {
        for (int y = 0; y * SIDE <= h; y++) {
//            if ((x & 1) ^ (y & 1)) {
//                draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xff0000);
//            }
            if (img_info.color_type==2){
                print("The image is RGB\n");
                pixel = (target_img[y*img_info.width+x]<<16) | (target_img[y*img_info.width+x+1]<<8) | (target_img[y*img_info.width+x+2]);
            }else{
                pixel = 0x000000;
            }
            draw_tile(x*SIDE, y*SIDE, SIDE, SIDE, pixel);
        }
    }
}



// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \"");
  puts(args);  // make run mainargs=xxx
  puts("\"\n");

//  splash();
    draw_image();
  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}
