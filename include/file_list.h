#pragma once
#ifndef __FILE_LIST_H
#define	__FILE_LIST_H
#include <all_header.h>


extern linklist_t create_file_link_init(void);
extern jpeg_linklist_t create_jpeg_link_init(void);
extern int list_for_all_file(const char *src_pathname,const linklist_t head_node);
extern int list_for_all_jpeg_file(const char *src_pathname,const jpeg_linklist_t g_jpeg_node);
extern int display_all_jpeg(const unsigned int *lcd_fb_ptr,const linklist_t g_head_node);
extern int right_display_picture(const unsigned int *lcd_fb_ptr,struct list_head *now_pos,const linklist_t g_head_node);
extern int display_main_material(const unsigned int *lcd_fb_ptr, const jpeg_linklist_t g_jpeg_node);
extern int display_mute_red(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node);
extern int display_mute_black(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node);
extern int display_lock_lock(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node);
extern int display_lock_unlock(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node);
#endif/*end #define	__FILE_LIST_H*/

