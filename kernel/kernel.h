//
// Created by liulingao on 24-4-12.
//

#ifndef OS_WORKBENCH_KERNEL_H
#define OS_WORKBENCH_KERNEL_H

#endif //OS_WORKBENCH_KERNEL_H
#include <am.h>
#include <klib.h>
// static void parse_png_image(image_data image, image_info * info){
//     info->width = (image[PNG_WIDTH_INDEX]<<24) | (image[PNG_WIDTH_INDEX+1]<<16) | (image[PNG_WIDTH_INDEX+2]<<8) | (image[PNG_WIDTH_INDEX+3]);
//     info->height = (image[PNG_HEIGHT_INDEX]<<24) | (image[PNG_HEIGHT_INDEX+1]<<16) | (image[PNG_HEIGHT_INDEX+2]<<8) | (image[PNG_HEIGHT_INDEX+3]);
//     info->color_depth = image[PNG_COLOR_TYPE_INDEX];
// }

static void parse_bmp_image(image_data image, image_info * info){
    info->width = (image[BMP_WIDTH_INDEX]<<24) | (image[BMP_WIDTH_INDEX+1]<<16) | (image[BMP_WIDTH_INDEX+2]<<8) | (image[BMP_WIDTH_INDEX+3]);
    info->height = (image[BMP_HEIGHT_INDEX]<<24) | (image[BMP_HEIGHT_INDEX+1]<<16) | (image[BMP_HEIGHT_INDEX+2]<<8) | (image[BMP_HEIGHT_INDEX+3]);
    info->color_depth = (image[BMP_COLOR_TYPE_INDEX]<<8) | (image[BMP_COLOR_TYPE_INDEX+1]);
}

image_info parse_image(image_data image){
    image_info info;
    if (image[0]=='B' && image[1]=='M'){ // bmp格式
        strcpy(info.format,"bmp");
        parse_bmp_image(image, &info); // 在函数内部修改info
    }else {
        println("unsupported image format"); // png的给我折磨傻了,先搁置一下,先做简单的bmp
    }
    return info;
}

// 最近邻插值函数
void nearestNeighborInterpolation(unsigned char *input, int inputWidth, int inputHeight,
                                  unsigned char *output, int outputWidth, int outputHeight)
{
    float scaleX = (float)inputWidth / outputWidth;
    float scaleY = (float)inputHeight / outputHeight;

    for (int y = 0; y < outputHeight; y++)
    {
        for (int x = 0; x < outputWidth; x++)
        {
            int srcX = (int)(x * scaleX);
            int srcY = (int)(y * scaleY);
            // 对于每个像素的RGBA四个通道，分别进行插值
            for (int c = 0; c < 4; c++)
            {
                output[(y * outputWidth + x) * 4 + c] = input[(srcY * inputWidth + srcX) * 4 + c];
                // println("index: %d, value: %d, src index: %d", (y * outputWidth + x) * 4 + c, output[(y * outputWidth + x) * 4 + c], (srcY * inputWidth + srcX) * 4 + c);
            }
        }
    }
}

void nearestNeighborInterpolation_RGB(unsigned char *input, int inputWidth, int inputHeight,
                                      unsigned char *output, int outputWidth, int outputHeight)
{
    printf("RGB: output width: %d, outputHeight: %d\n", outputWidth, outputHeight);
    float scaleX = (float)inputWidth / outputWidth;
    printf("scale: %.2f\n", scaleX);
    float scaleY = (float)inputHeight / outputHeight;

    for (int x = 0; x < outputWidth; x++)
    {
        for (int y = 0; y < outputHeight; y++)
        {
            int srcX = (int)(x * scaleX);
            int srcY = (int)(y * scaleY);
            // 对于每个像素的RGB三个通道，分别进行插值（此处实际上是复制）
            int dstIndex = (y * outputWidth + x) * 3;
            int srcIndex = (srcY * inputWidth + srcX) * 3;
            for (int c = 0; c < 3; c++)
            {
                output[dstIndex + c] = input[srcIndex + c];
            }
        }
    }
}