#pragma once
#ifndef  __JPEG_H
#define  __JPEG_H

#include <all_header.h>
extern int display_video_jpeg_picture(linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center);
extern int display_material_jpeg_picture(jpeg_linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center);
#endif
