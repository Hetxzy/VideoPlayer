/*************************************************
Copyright (C), 2018, XuRui
文件名: lcd.c
作者 : 许锐  Version: Date: 2018.08.17
功能描述: 存放对lcd的操作函数
// 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
其它内容的说明: 无
主要函数列表:
			void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_fb_ptr)
			void clean_main_right_frame_buf(unsigned int *lcd_fb_ptr)
			void clean_main_rate_frame_buf(unsigned int *lcd_fb_ptr)
			void redraw_main_rate_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
			void clean_main_sound_frame_buf(unsigned int *lcd_fb_ptr)
			void redraw_main_sound_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
			void clean_main_left_frame_buf(unsigned int *lcd_fb_ptr)
			void clean_frame_buf(unsigned int *lcd_fb_ptr)
			int open_lcd_device(unsigned int **lcd_fb_ptr)
			int close_lcd_device(int lcd_fd, unsigned int *lcd_fb_ptr,linklist_t head_node)

修改历史记录列表: 
1. Date:
Author:
Modification:
2. ...
*************************************************/
#include <lcd.h>
#define FB_SIZE	SCREEN_WIDTH_MAX*SCREEN_HEIGHT_MAX*4

/***************给一个点喷漆****************/
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_fb_ptr)
{
	*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y+x) = color;
}

/*********清空右屏幕，蓝屏*********/
void clean_main_right_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=0; y_s<SCREEN_HEIGHT_MAX; y_s++)
	{
		for(x_s=610; x_s<SCREEN_WIDTH_MAX; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
}
/*********清空进度条*********/
void clean_main_rate_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=370; y_s<400; y_s++)
	{
		for(x_s=0; x_s<450; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=25; x_s<425; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;
		}
	}

}
/**************重画视频进度条***************/
void redraw_main_rate_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
{
	int y_s,x_s;
	if (len > 425)
		len = 0;
	for(y_s=370; y_s<400; y_s++)
	{
		for(x_s=0; x_s<450; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;//淡蓝色
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=25; x_s<425; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;//青色
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=25; x_s<25+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; //深蓝色
		}
	}
	for(y_s=375; y_s<395; y_s++)
	{
		for(x_s=25+len-5; x_s<25+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; //深蓝色
		}
	}

}
/*********清空声音条*********/
void clean_main_sound_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=370; y_s<400; y_s++)
	{
		for(x_s=500; x_s<610; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=505; x_s<605; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;
		}
	}

}

/**************重画声音进度条***************/
void redraw_main_sound_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
{
	int y_s,x_s;
	for(y_s=370; y_s<400; y_s++)
	{
		for(x_s=500; x_s<610; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=505; x_s<605; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;
		}
	}
	for(y_s=380; y_s<390; y_s++)
	{
		for(x_s=505; x_s<505+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; 
		}
	}
	for(y_s=375; y_s<395; y_s++)
	{
		for(x_s=505+len-5; x_s<505+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; 
		}
	}

}

/*********清空全屏时的进度条*********/
void clean_full_rate_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=450; y_s<479; y_s++)
	{
		for(x_s=0; x_s<650; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD; //底色
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=25; x_s<625; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;//进度条颜色
		}
	}

}
/**************重画全屏时视频进度条***************/
void redraw_full_rate_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
{
	int y_s,x_s;
	if (len > 625)
		len = 0;
	for(y_s=450; y_s<479; y_s++)
	{
		for(x_s=0; x_s<650; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;//底色淡蓝色
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=25; x_s<625; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;//进度条底色 青色
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=25; x_s<25+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; //进度条 深蓝色
		}
	}
	for(y_s=455; y_s<475; y_s++)
	{
		for(x_s=25+len-5; x_s<25+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; //进度条方块 深蓝色
		}
	}

}
/*********清空全屏时声音条*********/
void clean_full_sound_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=450; y_s<479; y_s++)
	{
		for(x_s=680; x_s<799; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=690; x_s<790; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;
		}
	}

}

/**************重画全屏时声音进度条***************/
void redraw_full_sound_frame_buf(unsigned int *lcd_fb_ptr,unsigned int len)
{
	int y_s,x_s;
	for(y_s=450; y_s<479; y_s++)
	{
		for(x_s=680; x_s<799; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x008DB6CD;
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=690; x_s<790; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x0000868B;
		}
	}
	for(y_s=460; y_s<470; y_s++)
	{
		for(x_s=690; x_s<690+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; 
		}
	}
	for(y_s=455; y_s<475; y_s++)
	{
		for(x_s=690+len-5; x_s<690+len; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x000000EE; 
		}
	}

}
/*********清空左屏幕，黑屏*********/
void clean_main_left_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=400; y_s<SCREEN_HEIGHT_MAX; y_s++)
	{
		for(x_s=0; x_s<610; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0x00000000;
		}
	}
}
/*********清空屏幕，全白屏*********/
void clean_frame_buf(unsigned int *lcd_fb_ptr)
{
	int y_s,x_s;
	for(y_s=0; y_s<SCREEN_HEIGHT_MAX; y_s++)
	{
		for(x_s=0; x_s<SCREEN_WIDTH_MAX; x_s++)
		{
			*(lcd_fb_ptr+SCREEN_WIDTH_MAX*y_s+x_s) = 0xffffffff;
		}
	}
}
/************打开lcd，返回lcd文件描述符，并输出地址映射地址************/
int open_lcd_device(unsigned int **lcd_fb_ptr)
{
	int lcd_fd;

	lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd device failed\n");
		return -1;
	}

	*lcd_fb_ptr = mmap( NULL, FB_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(lcd_fb_ptr == MAP_FAILED)
	{
		perror("map lcd_fb error\n");
		return -1;
	}


	return lcd_fd;
}
/**********关闭lcd文件描述符，关闭内存映射地址************/
int close_lcd_device(int lcd_fd, unsigned int *lcd_fb_ptr,linklist_t head_node)
{
	
	
	linklist_t pos, n;
	list_for_each_entry_safe(pos, n, &(head_node->list), list)
	{
		printf("file name = %s\n",pos->info.file_name);  

		free(pos);
	}
	
	free(head_node);
	munmap(lcd_fb_ptr, FB_SIZE);

	return close(lcd_fd);
}

