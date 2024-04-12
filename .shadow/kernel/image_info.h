//
// Created by liulingao on 24-4-12.
//

#ifndef OS_WORKBENCH_IMAGE_INFO_H
#define OS_WORKBENCH_IMAGE_INFO_H

#endif //OS_WORKBENCH_IMAGE_INFO_H
#define PNG_WIDTH_INDEX 16
#define PNG_HEIGHT_INDEX 20
#define PNG_DATA_INDEX 29
#define PNG_COLOR_TYPE_INDEX 25
typedef struct {
    int width;
    int height;
    char format[10]; //"png" for now
    char color_type;
    /*0: 灰度，每个样本1个字节。
    2: 真彩色（RGB），每个像素3个字节。
    3: 真彩色带Alpha通道（RGBA），每个像素4个字节。
    4: 索引色，每个样本1个字节，使用PLTE（调色板）块。
    6: 索引色带Alpha通道，每个样本1个字节，使用PLTE和tRNS块。*/
}image_info;

typedef unsigned char* image_data;