//
// Created by liulingao on 24-4-12.
//

#ifndef OS_WORKBENCH_KERNEL_H
#define OS_WORKBENCH_KERNEL_H

#endif //OS_WORKBENCH_KERNEL_H
#include <am.h>
#include <klib.h>

image_info parse_png_image(image_data image){
    image_info info;
    strcpy(info.format,"png");
    printf(info.format);
    printf("\n");
    info.width = (image[PNG_WIDTH_INDEX]<<24) | (image[PNG_WIDTH_INDEX+1]<<16) | (image[PNG_WIDTH_INDEX+2]<<8) | (image[PNG_WIDTH_INDEX+3]);
    info.height = (image[PNG_HEIGHT_INDEX]<<24) | (image[PNG_HEIGHT_INDEX+1]<<16) | (image[PNG_HEIGHT_INDEX+2]<<8) | (image[PNG_HEIGHT_INDEX+3]);
    info.color_type = image[PNG_COLOR_TYPE_INDEX];

    return info;
}

// 双线性插值函数
void bilinear_interpolation(unsigned char *src_pixels, int src_width, int src_height,
                                unsigned char *dst_pixels, int dst_width, int dst_height)
{
    // 对目标图像的每个像素进行双线性插值
    for (int dst_y = 0; dst_y < dst_height; ++dst_y)
    {
        printf("dst_y:%d\n",dst_y);
        for (int dst_x = 0; dst_x < dst_width; ++dst_x)
        {
            printf("dst_x:%d\n",dst_x);
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