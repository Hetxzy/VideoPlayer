/*************************************************
Copyright (C), 2018, XuRui
文件名: display_jpeg.c
作者 : 许锐  Version: Date: 2018.08.17
功能描述: 存放对jpeg图片的打印函数，包括特效打印
// 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
其它内容的说明: 无
主要函数列表:
draw_lcd_jpeg_normal(unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)：普通打印
draw_lcd_jpeg_window_shades_up (unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)：雪花打印
draw_lcd_jpeg_window_shades (unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)：百叶窗打印
int display_material_jpeg_picture(jpeg_linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center)
int display_video_jpeg_picture(linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center)

修改历史记录列表: 
1. Date:
Author:
Modification:
2. ...
*************************************************/
#include <display_jpeg.h>

extern JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
extern int image_height;	/* Number of rows in image */
extern int image_width;		/* Number of columns in image */

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/*************************************************
函数名称:   draw_lcd_jpeg_normal   
功能: 普通的打印图片的效果
被本函数调用的函数清单: 
					lcd.c->lcd_draw_point(x, y, color, lcd_ptr);
调用本函数的函数清单: 
					display_jpeg_picture(linklist ptr,unsigned int *lcd_ptr)
输入参数说明，包括每个参数的作用、取值说明及参数间关系：
					y_s:高度开始行 x_s:宽度开始列 y_end:高度结束行 x_ends:宽度结束列
					new_pic_buf:图片像素数据  lcd_ptr：lcd内存映射地址
输出参数: lcd_ptr ：改变内存映射的地址指向的内存的值，就可在lcd上显示图片 
函数返回值: 无
其他说明: 无
*************************************************/
void draw_lcd_jpeg_normal(unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)
{
	char *draw_ptr;
	unsigned int color;
	unsigned int x, y;
	unsigned int new_width = x_end -x_s ;
	for(y=y_s; y<y_end && y<SCREEN_HEIGHT_MAX; y++)
	{

		draw_ptr = new_pic_buf + ((y-y_s)*(new_width)*3);
	
		for( x=x_s; x<x_end; x++)
		{

			if(x<SCREEN_WIDTH_MAX)
			{

				color = (*(draw_ptr+2) )| (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}

			draw_ptr += 3;
			
		}

	}
}
/*************************************************
函数名称:   draw_lcd_jpeg_window_shades_up   
功能: 实现以从下到上的百叶窗打印图片的效果
被本函数调用的函数清单: 
					lcd.c->lcd_draw_point(x, y, color, lcd_ptr);
调用本函数的函数清单: 
					display_jpeg_picture(linklist ptr,unsigned int *lcd_ptr)
输入参数说明，包括每个参数的作用、取值说明及参数间关系：
					y_s:高度开始行 x_s:宽度开始列 y_end:高度结束行 x_ends:宽度结束列
					new_pic_buf:图片像素数据  lcd_ptr：lcd内存映射地址
输出参数: lcd_ptr ：改变内存映射的地址指向的内存的值，就可在lcd上显示图片 
函数返回值: 无
其他说明: 无
*************************************************/
void draw_lcd_jpeg_window_shades_up (unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)
{
	char *draw_ptr;
	unsigned int color;
	unsigned int x, y;
	int cnt_end = (y_end - y_s)/4;
	int cnt = 0;
	unsigned int new_width = x_end - x_s;
	unsigned int y_start_save = y_s;
	while(cnt < cnt_end)
	{
		usleep(1000);
		y = y_end;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_end-cnt_end*1;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_end-cnt_end*2;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_end-cnt_end*3;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		cnt++;
		y_end--;
	}

}
/*************************************************
函数名称:   draw_lcd_jpeg_window_shades   
功能: 实现以从上到下百叶窗打印图片的效果
被本函数调用的函数清单: 
					lcd.c->lcd_draw_point(x, y, color, lcd_ptr);
调用本函数的函数清单: 
					display_jpeg_picture(linklist ptr,unsigned int *lcd_ptr)
输入参数说明，包括每个参数的作用、取值说明及参数间关系：
					y_s:高度开始行 x_s:宽度开始列 y_end:高度结束行 x_ends:宽度结束列
					new_pic_buf:图片像素数据  lcd_ptr：lcd内存映射地址
输出参数: lcd_ptr ：改变内存映射的地址指向的内存的值，就可在lcd上显示图片 
函数返回值: 无
其他说明: 无
*************************************************/
void draw_lcd_jpeg_window_shades (unsigned int y_s,unsigned int y_end,unsigned int x_s,unsigned int x_end, char *new_pic_buf,unsigned int *lcd_ptr)
{
	char *draw_ptr;
	unsigned int color;
	unsigned int x, y;
	int cnt_end = (y_end - y_s)/4;
	int cnt = 0;
	unsigned int new_width = x_end - x_s;
	unsigned int y_start_save = y_s;
	while(cnt < cnt_end)
	{
		usleep(1000);
		y = y_s;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_s+cnt_end*1;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_s+cnt_end*2;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		
		y = y_s+cnt_end*3;
		draw_ptr = new_pic_buf + ((y-y_start_save)*(new_width)*3);
		for( x=x_s; x<x_end; x++)
		{
			if(x<SCREEN_WIDTH_MAX)
			{
				color = (*(draw_ptr+2)) | (*(draw_ptr+1)<<8) |(*(draw_ptr)<<16);
				lcd_draw_point(x, y, color, lcd_ptr);
			}
			draw_ptr += 3;
		}
		cnt++;
		y_s++;
	}

}
/*************************************************
函数名称:   display_video_jpeg_picture   
功能: 根据不同指令运动不同特效显示bmp图片,并将图片的宽度高度改为匹配lcd的宽度高度，实现图片基于lcd的自动缩放
被本函数调用的函数清单: 
					draw_lcd_jpeg_snow(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
					draw_lcd_jpeg_window_shades(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
					draw_lcd_jpeg_normal(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
调用本函数的函数清单: file_list.c->right_display_picture
					file_list.c->int display_all_jpeg(const unsigned int *lcd_fb_ptr,const linklist_t g_head_node)
输入参数说明，包括每个参数的作用、取值说明及参数间关系：ptr：指向包含文件信息的链表结构体的指针  lcd_ptr：lcd内存映射地址
输出参数: lcd_ptr ：改变内存映射的地址指向的内存的值，就可在lcd上显示图片
函数返回值: 0：成功 -1：失败
其他说明: 无
*************************************************/
int display_video_jpeg_picture(linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center)
{
	unsigned int x_s,y_s,x_end,y_end;
	unsigned int new_height,new_width;
	off_t new_file_size;//缩小的图像的大小
	char *new_pic_buf; //存储缩小的图像的信息的地址
	char *new_pir_dest,*old_pir_src;//用于复制缩放数据时的指针
	unsigned long dwsrcX,dwsrcY; //这个是用于保存x和y的缩放比例
	char *draw_ptr;//指向需要喷漆的图片的信息源
	int i,j;
	/****************将图片的宽度高度改为匹配lcd的宽度高度，实现图片基于lcd的自动缩放*******************
	if(ptr->info.pictureInfo.picture_width > SCREEN_WIDTH_MAX && ptr->info.pictureInfo.picture_height > SCREEN_HEIGHT_MAX)
	{
		new_width = SCREEN_WIDTH_MAX;
		new_height = (ptr->info.pictureInfo.picture_height)*(new_width)/(ptr->info.pictureInfo.picture_width);
		if(new_height > SCREEN_HEIGHT_MAX)
			new_height = SCREEN_HEIGHT_MAX;
		
	}
	else if(ptr->info.pictureInfo.picture_width > SCREEN_WIDTH_MAX && ptr->info.pictureInfo.picture_height <= SCREEN_HEIGHT_MAX)
	{
		new_width = SCREEN_WIDTH_MAX;
		new_height = (ptr->info.pictureInfo.picture_height)*(new_width)/(ptr->info.pictureInfo.picture_width);
		
	}
	else if (ptr->info.pictureInfo.picture_width <= SCREEN_WIDTH_MAX && ptr->info.pictureInfo.picture_height > SCREEN_HEIGHT_MAX)
	{
	
		new_height = SCREEN_HEIGHT_MAX;
		new_width = (ptr->info.pictureInfo.picture_width)*(new_height)/(ptr->info.pictureInfo.picture_height);
		

	}
	else if (ptr->info.pictureInfo.picture_width <= SCREEN_WIDTH_MAX && ptr->info.pictureInfo.picture_height <= SCREEN_HEIGHT_MAX) 
	{
		new_width = SCREEN_WIDTH_MAX;
		new_height = (ptr->info.pictureInfo.picture_height)*(new_width)/(ptr->info.pictureInfo.picture_width);
		
		if(new_height > SCREEN_HEIGHT_MAX)
			{
				new_height = SCREEN_HEIGHT_MAX;
				new_width = (ptr->info.pictureInfo.picture_width)*(new_height)/(ptr->info.pictureInfo.picture_height);
			}
	}*/

	new_width = width;
	new_height = height;
	/*************创建新的动态内存，并将旧的像素数据按比例赋给新的动态内存****************/
	new_width = new_width - (new_width % 4);
	new_file_size = new_width*new_height*3;
	new_pic_buf = malloc(new_file_size);
	if(new_pic_buf == NULL)
	{
		perror("alloc new_pic_buf memory error\n");
		return -1;
	}

	for(i=0;i<new_height;i++)
	{
		dwsrcY = i*(ptr->info.pictureInfo.picture_height)/new_height;
		new_pir_dest = new_pic_buf + i*new_width*3;
		old_pir_src = (ptr->info.pictureInfo.picture_buf) + dwsrcY*(ptr->info.pictureInfo.picture_width)*3;
		for(j=0;j<new_width;j++)
		{
			dwsrcX = j*(ptr->info.pictureInfo.picture_width)/new_width;
			memcpy(new_pir_dest+j*3,old_pir_src+dwsrcX*3,3);
		}
	}
	/*****************居中打印图片，并根据不同触屏指令选择不同的特效打印********************/
	x_s = x_center- (new_width)/2;
	y_s = y_center - (new_height)/2;
	x_end = x_center + (new_width)/2;
	y_end = y_center + (new_height)/2; 
	if (g_wPrint_type == 1 )
	{
		draw_lcd_jpeg_window_shades(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
	}
	if (g_wPrint_type == 2 )
	{
		draw_lcd_jpeg_window_shades_up(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
	}
	if (g_wPrint_type == 0)
	{
		draw_lcd_jpeg_normal(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
	}
	/****************得到新的宽度，高度，像素数据******************/
	ptr->info.pictureInfo.picture_width = new_width;
	ptr->info.pictureInfo.picture_height = new_height;
	char* tmp_buf;
	tmp_buf = ptr->info.pictureInfo.picture_buf;
	free(tmp_buf);
	ptr->info.pictureInfo.picture_buf = new_pic_buf;
}

/*************************************************
函数名称:   display_material_jpeg_picture   
功能: 根据参数打印jpeg图片
被本函数调用的函数清单: 
					draw_lcd_jpeg_normal(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
调用本函数的函数清单: file_list.c->display_main_material

输入参数说明，包括每个参数的作用、取值说明及参数间关系：ptr：指向包含文件信息的链表结构体的指针  lcd_ptr：lcd内存映射地址
输出参数: lcd_ptr ：改变内存映射的地址指向的内存的值，就可在lcd上显示图片
函数返回值: 0：成功 -1：失败
其他说明: 无
*************************************************/

int display_material_jpeg_picture(jpeg_linklist_t ptr,unsigned int *lcd_ptr,unsigned int width,unsigned int height,unsigned int x_center,unsigned int y_center)
{
	unsigned int x_s,y_s,x_end,y_end;
	unsigned int new_height,new_width;
	off_t new_file_size;//缩小的图像的大小
	char *new_pic_buf; //存储缩小的图像的信息的地址
	char *new_pir_dest,*old_pir_src;//用于复制缩放数据时的指针
	unsigned long dwsrcX,dwsrcY; //这个是用于保存x和y的缩放比例
	char *draw_ptr;//指向需要喷漆的图片的信息源
	int i,j;

	new_width = width;
	new_height = height;
	/*************创建新的动态内存，并将旧的像素数据按比例赋给新的动态内存****************/
	new_width = new_width - (new_width % 4);
	new_file_size = new_width*new_height*3;
	new_pic_buf = malloc(new_file_size);
	if(new_pic_buf == NULL)
	{
		perror("alloc new_pic_buf memory error\n");
		return -1;
	}

	for(i=0;i<new_height;i++)
	{
		dwsrcY = i*(ptr->info.picture_height)/new_height;
		new_pir_dest = new_pic_buf + i*new_width*3;
		old_pir_src = (ptr->info.picture_buf) + dwsrcY*(ptr->info.picture_width)*3;
		for(j=0;j<new_width;j++)
		{
			dwsrcX = j*(ptr->info.picture_width)/new_width;
			memcpy(new_pir_dest+j*3,old_pir_src+dwsrcX*3,3);
		}
	}
	/*****************居中打印图片，并根据不同触屏指令选择不同的特效打印********************/
	x_s = x_center- (new_width)/2;
	y_s = y_center - (new_height)/2;
	x_end = x_center + (new_width)/2;
	y_end = y_center + (new_height)/2; 
	
	draw_lcd_jpeg_normal(y_s,y_end,x_s,x_end,new_pic_buf,lcd_ptr);
	
	/****************得到新的宽度，高度，像素数据******************/
	ptr->info.picture_width = new_width;
	ptr->info.picture_height = new_height;
	char* tmp_buf;
	tmp_buf = ptr->info.picture_buf;
	free(tmp_buf);
	ptr->info.picture_buf = new_pic_buf;
}