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