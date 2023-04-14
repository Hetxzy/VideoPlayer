#pragma once
#ifndef __LCD_H
#define	__LCD_H
#include <all_header.h>

extern void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_fb_ptr);
extern void clean_frame_buf(unsigned int *lcd_fb_ptr);
extern void clean_main_right_frame_buf(unsigned int *lcd_fb_ptr);
extern void clean_main_left_frame_buf(unsigned int *lcd_fb_ptr);

extern void clean_main_rate_frame_buf(unsigned int *lcd_fb_ptr);
extern void clean_main_sound_frame_buf(unsigned int *lcd_fb_ptr);
extern void redraw_main_sound_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len);
extern void redraw_main_rate_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len);

extern void clean_full_rate_frame_buf(unsigned int *lcd_fb_ptr);
extern void clean_full_sound_frame_buf(unsigned int *lcd_fb_ptr);
extern void redraw_full_sound_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len);
extern void redraw_full_rate_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len);

extern int open_lcd_device(unsigned int **lcd_fb_ptr);
extern int close_lcd_device(int lcd_fd, unsigned int *lcd_fb_ptr,linklist_t head_node);

#endif/*end define __LCD_H*/
