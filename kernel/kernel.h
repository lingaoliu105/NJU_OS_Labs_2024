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

// 双线性插值函数
void bilinear_interpolation(unsigned char *src_pixels, int src_width, int src_height,
                                unsigned char *dst_pixels, int dst_width, int dst_height)
{
    // 对目标图像的每个像素进行双线性插值
    for (int dst_y = 0; dst_y < dst_height; ++dst_y)
    {
        // if (dst_y % 10 == 0)
        // {
        //     printf("dst_y:%d\n",dst_y);
        // }
        for (int dst_x = 0; dst_x < dst_width; ++dst_x)
        {
            // printf("dst_x:%d\n",dst_x);
            // 计算源图像中待插值的四个像素坐标
            const int src_x0 = dst_x / 2, src_x1 = src_x0 + 1;
            const int src_y0 = dst_y / 2, src_y1 = src_y0 + 1;

            // 检查坐标是否越界，此处简化处理，实际应用中应添加边界条件检查
            if (src_x0 < 0 || src_x1 >= src_width || src_y0 < 0 || src_y1 >= src_height)
            {
                // 若越界，可选择填充默认颜色或忽略该像素
                continue;
            }

            // 计算四个像素在源图像中的偏移
            const int offset_x0 = src_x0 * 3, offset_x1 = src_x1 * 3;
            const int offset_y0 = src_y0 * src_width * 3, offset_y1 = src_y1 * src_width * 3;

            // 计算双线性插值权重
            const float wx = dst_x % 2 / 2.0f, wy = dst_y % 2 / 2.0f;

            // 计算目标像素的颜色
            for (int channel = 0; channel < 3; ++channel)
            {
                const unsigned char p00 = src_pixels[offset_y0 + offset_x0 + channel];
                const unsigned char p01 = src_pixels[offset_y0 + offset_x1 + channel];
                const unsigned char p10 = src_pixels[offset_y1 + offset_x0 + channel];
                const unsigned char p11 = src_pixels[offset_y1 + offset_x1 + channel];

                const float interpolated_value =
                    (1.0f - wx) * ((1.0f - wy) * p00 + wy * p10) +
                    wx * ((1.0f - wy) * p01 + wy * p11);

                dst_pixels[dst_y * dst_width * 3 + dst_x * 3 + channel] =
                    (unsigned char)(interpolated_value + 0.5f); // 四舍五入到最接近的整数
            }
        }
    }
}

void bilinear_interpolation_rgba(unsigned char *src_pixels, int src_width, int src_height,
                                 unsigned char *dst_pixels, int dst_width, int dst_height)
{
    // 对目标图像的每个像素进行双线性插值
    for (int dst_y = 0; dst_y < dst_height; ++dst_y)
    {
        for (int dst_x = 0; dst_x < dst_width; ++dst_x)
        {
            // 计算源图像中待插值的四个像素坐标
            const int src_x0 = dst_x / 2, src_x1 = src_x0 + 1;
            const int src_y0 = dst_y / 2, src_y1 = src_y0 + 1;

            // 检查坐标是否越界，此处简化处理，实际应用中应添加边界条件检查
            if (src_x0 < 0 || src_x1 >= src_width || src_y0 < 0 || src_y1 >= src_height)
            {
                // 若越界，可选择填充默认颜色或忽略该像素
                continue;
            }

            // 计算四个像素在源图像中的偏移
            const int offset_x0 = src_x0 * 4, offset_x1 = src_x1 * 4;
            const int offset_y0 = src_y0 * src_width * 4, offset_y1 = src_y1 * src_width * 4;

            // 计算双线性插值权重
            const float wx = dst_x % 2 / 2.0f, wy = dst_y % 2 / 2.0f;

            // 计算目标像素的颜色和透明度
            for (int channel = 0; channel < 4; ++channel)
            {
                const unsigned char p00 = src_pixels[offset_y0 + offset_x0 + channel];
                const unsigned char p01 = src_pixels[offset_y0 + offset_x1 + channel];
                const unsigned char p10 = src_pixels[offset_y1 + offset_x0 + channel];
                const unsigned char p11 = src_pixels[offset_y1 + offset_x1 + channel];

                const float interpolated_value =
                    (1.0f - wx) * ((1.0f - wy) * p00 + wy * p10) +
                    wx * ((1.0f - wy) * p01 + wy * p11);

                dst_pixels[dst_y * dst_width * 4 + dst_x * 4 + channel] =
                    (unsigned char)(interpolated_value + 0.5f); // 四舍五入到最接近的整数
            }
        }
    }
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