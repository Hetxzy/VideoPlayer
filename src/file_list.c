/*************************************************
Copyright (C), 2018, XuRui
文件名: file_list.c
作者 : 许锐  Version: Date: 2018.08.17
功能描述: 用于存放对照片的操作，如如何打印，打印下一张或者上一张
// 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
其它内容的说明: 无
主要函数列表:
			linklist_t create_file_link_init(void)
			jpeg_linklist_t create_jpeg_link_init(void)
			int list_for_all_file(const char *src_pathname,const linklist_t head_node)
			int list_for_all_jpeg_file(const char *src_pathname,const jpeg_linklist_t g_jpeg_node)
			int display_all_jpeg(const unsigned int *lcd_fb_ptr,const linklist_t g_head_node)
			int display_main_material(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
			int display_mute_black(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
			int display_mute_red(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
			char *get_name(char *name)
			void print_char(char *name,const unsigned int *lcd_fb_ptr,unsigned int x,unsigned int y)
			int right_display_picture(const unsigned int *lcd_fb_ptr,const struct list_head *now_pos,const linklist_t g_head_node)
修改历史记录列表: 
1. Date:
Author:
Modification:
2. ...
*************************************************/


#include <file_list.h>




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
/***********创建保存视频文件信息的链表并初始化************/
linklist_t create_file_link_init(void)
{
	linklist_t head_node;

	head_node = malloc(sizeof(linknode_t));
	if(head_node == NULL)
		return NULL;

	INIT_LIST_HEAD(&(head_node->list));

	return head_node;
}
/***********创建保存jpeg文件信息的链表并初始化************/
jpeg_linklist_t create_jpeg_link_init(void)
{
	jpeg_linklist_t head_node;

	head_node = malloc(sizeof(jpeg_linknode_t));
	if(head_node == NULL)
		return NULL;

	INIT_LIST_HEAD(&(head_node->list));

	return head_node;
}

/*************************************************
函数名称:   list_for_all_file   
功能: 递归遍历指定目录里的所有子目录下的视频文件，获取MMP4、avi等视频文件信息并保存在链表中，同时截取一帧图片保存
被本函数调用的函数清单: 
					list_for_all_file(name_buf,head_node);
调用本函数的函数清单: 
					main()
输入参数说明，包括每个参数的作用、取值说明及参数间关系：
					src_pathname：指定路径名 head_node:指向包含文件信息的链表结构体的头指针
输出参数: 无
函数返回值: 无
其他说明: 无
*************************************************/
int list_for_all_file(const char *src_pathname,const linklist_t head_node)
{
	int retval;
	struct dirent *file_info; //目录结构体
	DIR *dir_ptr; //目录指针
	char name_buf[50];
	/*************打开目录**************/
	dir_ptr = opendir(src_pathname);
	if(dir_ptr == NULL)
	{
		perror("open src dir failed\n");
		return -1;
	}

		while(1)
	{
		
		file_info = readdir(dir_ptr);
		if(file_info == NULL)
		{
			break;
		}
		sprintf(name_buf,"%s/%s",src_pathname,file_info->d_name);//合并目录名称


		if(file_info->d_type == DT_DIR ) //如果是文件是目录
		{
			if( (strcmp(file_info->d_name,".") != 0) && (strcmp(file_info->d_name,"..") != 0) )
			{
				list_for_all_file(name_buf,head_node);
			}
			continue;//该文件是目录，则遍历下一个文件
		}

		if(file_info->d_type == DT_REG ) //如果是普通文件
		{
			
			
			/***********获得mp4的信息********/
			
			if(strstr(file_info->d_name,".mp4") != NULL)
			{
				linklist_t new_node;
	
				new_node = malloc(sizeof(linknode_t));
				if(new_node == NULL)
					return -1;
		
				strcpy(new_node->info.file_name,name_buf);//获取文件名字
				printf("file name = %s\n", new_node->info.file_name);
				strcpy(new_node->info.file_type,"MP4");//设置文件类型
				printf("file type = %s\n", new_node->info.file_type);
				/************打开子进程，获取一帧图片************/
				pid_t pid;
				pid = vfork();
				if(pid == 0)
				{
		
					execlp("mplayer","mplayer","-ss","84","-noframedrop","-nosound","-vo","jpeg","-frames","1","%s",new_node->info.file_name,NULL);
		
					exit(0);
				}

				waitpid(pid,NULL,0);

				struct dirent *find_jpeg_dirent; //目录结构体
				DIR *find_jpeg_ptr; //目录指针
				char fing_jpeg_buf[50];
				/**********子进程获取的图片保存在当前目录下，故遍历当前目录即可找到***********/
				find_jpeg_ptr = opendir(".");
				if(find_jpeg_ptr == NULL)
				{
					perror("open src dir failed\n");
					return -1;
				}

				while(1)
				{
		
					find_jpeg_dirent = readdir(find_jpeg_ptr);
					if(find_jpeg_dirent == NULL)
					{
						break;
					}
					//sprintf(fing_jpeg_buf,"%s",file_info->d_name);
					if(find_jpeg_dirent->d_type == DT_REG ) //如果是普通文件
					{
						/************子进程获取的jpeg图片结尾为01.jpg**************/
						if(strstr(find_jpeg_dirent->d_name,"01.jpg") != NULL)
						{
							sprintf(fing_jpeg_buf,"./%s",find_jpeg_dirent->d_name);

							/***********获得一帧jpeg图片后提取信息**********/
							struct jpeg_decompress_struct cinfo;
  
							struct my_error_mgr jerr;
							FILE * infile;		/* source file */
							int row_stride;		/* physical row width in output buffer */
							/***********打开文件并判断返回值************/
							if((infile = fopen(fing_jpeg_buf, "rb")) == NULL) 
							{
								fprintf(stderr, "can't open %s\n", fing_jpeg_buf);
								return 0;
							}
							/***********出错就调用my_error_exit************/
							cinfo.err = jpeg_std_error(&jerr.pub);
							jerr.pub.error_exit = my_error_exit;
  			
							if(setjmp(jerr.setjmp_buffer)) 
							{    
								jpeg_destroy_decompress(&cinfo);
								fclose(infile);
								return 0;
							}



							/******解压图片，等到图片信息info*******/
							jpeg_create_decompress(&cinfo);

							jpeg_stdio_src(&cinfo, infile);
	
							jpeg_read_header(&cinfo, TRUE);
    
							jpeg_start_decompress(&cinfo);
  
							row_stride = cinfo.output_width * cinfo.output_components;
							/************存储图片像素信息的动态内存************/
							char *buffer;		/* Output row buffer */
				
				
				
							new_node->info.pictureInfo.picture_height = cinfo.output_height;
							printf("picture_height = %u\n", new_node->info.pictureInfo.picture_height);
							new_node->info.pictureInfo.picture_width = cinfo.output_width;
							printf("picture_width = %u\n", new_node->info.pictureInfo.picture_width);
							unsigned long rgb_size;
							rgb_size = new_node->info.pictureInfo.picture_height * new_node->info.pictureInfo.picture_width*3;
							buffer = malloc(rgb_size);
							new_node->list.prev = NULL;
							new_node->list.next = NULL;
							/*************获取该jpeg图片的像素数据**********/
							while(cinfo.output_scanline < cinfo.output_height) 
							{    
								unsigned char *buffer_array[1];
								buffer_array[0] = buffer + (cinfo.output_scanline) * row_stride;
								jpeg_read_scanlines(&cinfo, buffer_array, 1);
							}

							new_node->info.pictureInfo.picture_buf = buffer;
							list_add_tail(&(new_node->list), &(head_node->list));
				
 

							jpeg_finish_decompress(&cinfo);
 
							jpeg_destroy_decompress(&cinfo);

 
							fclose(infile);
						}//end of if(strstr(fing_jpeg_buf,"01.jpg") != NULL)

					}//end of if(file_info->d_type == DT_REG )


				}//end of while 1
			}//end of if(strstr(name_buf,".mp4") != NULL)


			/**************获得avi视频文件信息，原理同MP4*****************/
			if(strstr(file_info->d_name,".avi") != NULL)
			{
				linklist_t new_node;
	
				new_node = malloc(sizeof(linknode_t));
				if(new_node == NULL)
					return -1;
		
				strcpy(new_node->info.file_name,name_buf);
				printf("file name = %s\n", new_node->info.file_name);
				strcpy(new_node->info.file_type,"AVI");
				printf("file type = %s\n", new_node->info.file_type);
				pid_t pid;
				pid = vfork();
				if(pid == 0)
				{
		
					execlp("mplayer","mplayer","-ss","84","-noframedrop","-nosound","-vo","jpeg","-frames","1","%s",new_node->info.file_name,NULL);
		
					exit(0);
				}

				waitpid(pid,NULL,0);

				struct dirent *find_jpeg_dirent; //目录结构体
				DIR *find_jpeg_ptr; //目录指针
				char fing_jpeg_buf[50];
				find_jpeg_ptr = opendir(".");
				if(find_jpeg_ptr == NULL)
				{
					perror("open src dir failed\n");
					return -1;
				}

				while(1)
				{
		
					find_jpeg_dirent = readdir(find_jpeg_ptr);
					if(find_jpeg_dirent == NULL)
					{
						break;
					}
					//sprintf(fing_jpeg_buf,"%s",file_info->d_name);
					if(find_jpeg_dirent->d_type == DT_REG ) //如果是普通文件
					{
						if(strstr(find_jpeg_dirent->d_name,"01.jpg") != NULL)
						{
							sprintf(fing_jpeg_buf,"./%s",find_jpeg_dirent->d_name);

							/***********获得一帧jpeg图片后提取信息**********/
							struct jpeg_decompress_struct cinfo;
  
							struct my_error_mgr jerr;
							FILE * infile;		/* source file */
							int row_stride;		/* physical row width in output buffer */
							/***********打开文件并判断返回值************/
							if((infile = fopen(fing_jpeg_buf, "rb")) == NULL) 
							{
								fprintf(stderr, "can't open %s\n", fing_jpeg_buf);
								return 0;
							}
							/***********出错就调用my_error_exit************/
							cinfo.err = jpeg_std_error(&jerr.pub);
							jerr.pub.error_exit = my_error_exit;
  			
							if(setjmp(jerr.setjmp_buffer)) 
							{    
								jpeg_destroy_decompress(&cinfo);
								fclose(infile);
								return 0;
							}



							/******解压图片，等到图片信息info*******/
							jpeg_create_decompress(&cinfo);

							jpeg_stdio_src(&cinfo, infile);
	
							jpeg_read_header(&cinfo, TRUE);
    
							jpeg_start_decompress(&cinfo);
  
							row_stride = cinfo.output_width * cinfo.output_components;
							/************存储图片像素信息的动态内存************/
							char *buffer;		/* Output row buffer */
				
				
				
							new_node->info.pictureInfo.picture_height = cinfo.output_height;
							printf("picture_height = %u\n", new_node->info.pictureInfo.picture_height);
							new_node->info.pictureInfo.picture_width = cinfo.output_width;
							printf("picture_width = %u\n", new_node->info.pictureInfo.picture_width);
							unsigned long rgb_size;
							rgb_size = new_node->info.pictureInfo.picture_height * new_node->info.pictureInfo.picture_width*3;
							buffer = malloc(rgb_size);
							new_node->list.prev = NULL;
							new_node->list.next = NULL;
				
							while(cinfo.output_scanline < cinfo.output_height) 
							{    
								unsigned char *buffer_array[1];
								buffer_array[0] = buffer + (cinfo.output_scanline) * row_stride;
								jpeg_read_scanlines(&cinfo, buffer_array, 1);
							}

							new_node->info.pictureInfo.picture_buf = buffer;
							list_add_tail(&(new_node->list), &(head_node->list));
				
 

							jpeg_finish_decompress(&cinfo);
 
							jpeg_destroy_decompress(&cinfo);

 
							fclose(infile);
						}//end of if(strstr(fing_jpeg_buf,"01.jpg") != NULL)

					}//end of if(file_info->d_type == DT_REG )


				}//end of while 1
			}//end of if(strstr(name_buf,".mp4") != NULL)
		}//end of if(file_info->d_type == DT_REG ) 

	}//end of while 1

	closedir(dir_ptr);

	return 0;
}
/*************************************************
函数名称:   list_for_all_jpeg_file   
功能: 递归遍历指定目录里的所有子目录下的jpeg文件，获取jpeg图片文件信息并保存在链表中
被本函数调用的函数清单: 
					list_for_all_jpeg_file(name_buf,head_node);
调用本函数的函数清单: 
					main()
输入参数说明，包括每个参数的作用、取值说明及参数间关系：
					src_pathname：指定路径名 head_node:指向包含文件信息的链表结构体的头指针
输出参数: 无
函数返回值: 无
其他说明: 无
*************************************************/

int list_for_all_jpeg_file(const char *src_pathname,const jpeg_linklist_t g_jpeg_node)
{
	struct stat statbuf;//文件属性
	int retval;
	struct dirent *file_info;//目录结构体
	DIR *dir_ptr;//目录指针
	char name_buf[50];

	/*********打开目录********/
	dir_ptr = opendir(src_pathname);
	if(dir_ptr == NULL)
	{
		perror("open src dir failed\n");
		return -1;
	}

		while(1)
	{
		/***********读取目录，获取目录结构体，目录指针下移**********/
		file_info = readdir(dir_ptr);
		if(file_info == NULL)
		{
			break;
		}
		sprintf(name_buf,"%s/%s",src_pathname,file_info->d_name);

		/*********是目录，则递归*********/
		if(file_info->d_type == DT_DIR )
		{
			if( (strcmp(file_info->d_name,".") != 0) && (strcmp(file_info->d_name,"..") != 0) )
			{
				list_for_all_jpeg_file(name_buf,g_jpeg_node);
			}
			continue;
		}
		/**********如果是普通文件**********/
		if(file_info->d_type == DT_REG )
		{
			retval = stat(name_buf, &statbuf);
			if(retval == -1)
			{
				perror("get file status error\n");
				return -1;
			}
			
			/***********获得jpeg图片的信息********/
			if(strstr(name_buf,".jpg") != NULL)
			{
				jpeg_linklist_t new_node;
	    
				new_node = malloc(sizeof(jpeg_linknode_t));
				if(new_node == NULL)
					return -1;
		
				strcpy(new_node->info.file_name,name_buf);
				printf("file name = %s\n", new_node);
				struct jpeg_decompress_struct cinfo;
  
				struct my_error_mgr jerr;
				FILE * infile;		/* source file */
				int row_stride;		/* physical row width in output buffer */
				/***********打开文件并判断返回值************/
				if((infile = fopen(name_buf, "rb")) == NULL) 
				{
					fprintf(stderr, "can't open %s\n", name_buf);
					return 0;
				}
				/***********出错就调用my_error_exit************/
				cinfo.err = jpeg_std_error(&jerr.pub);
				jerr.pub.error_exit = my_error_exit;
  
				if(setjmp(jerr.setjmp_buffer)) 
				{    
					jpeg_destroy_decompress(&cinfo);
					fclose(infile);
					return 0;
				}



				/******解压图片，等到图片信息info*******/
				jpeg_create_decompress(&cinfo);

				jpeg_stdio_src(&cinfo, infile);
	
				jpeg_read_header(&cinfo, TRUE);
    
				jpeg_start_decompress(&cinfo);
  
				row_stride = cinfo.output_width * cinfo.output_components;
				/************存储图片像素信息的动态内存************/
				char *buffer;		/* Output row buffer */
				
				//buffer =(char *)(*cinfo.mem->alloc_sarray)
					//((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
				
				new_node->info.picture_height = cinfo.output_height;

				new_node->info.picture_width = cinfo.output_width;
				unsigned long rgb_size;
				rgb_size = new_node->info.picture_height * new_node->info.picture_width*3;
				buffer = malloc(rgb_size);
				new_node->list.prev = NULL;
				new_node->list.next = NULL;
				
				while(cinfo.output_scanline < cinfo.output_height) 
				{    
					unsigned char *buffer_array[1];
					buffer_array[0] = buffer + (cinfo.output_scanline) * row_stride;
					jpeg_read_scanlines(&cinfo, buffer_array, 1);
					
					
				}

				new_node->info.picture_buf = buffer;
				list_add_tail(&(new_node->list), &(g_jpeg_node->list));
				
 

				jpeg_finish_decompress(&cinfo);
 
				jpeg_destroy_decompress(&cinfo);

 
				fclose(infile);

				
			}//end of if(strstr(name_buf,".jpg") != NULL)
		
		}//end of if(file_info->d_type == DT_REG )
		

	}
	closedir(dir_ptr);

	return 0;
}



/********************逐一显示所有一阵截图***********************/
int display_all_jpeg(const unsigned int *lcd_fb_ptr,const linklist_t g_head_node)
{
	struct list_head *pos;
	linklist_t ptr;
	clean_main_right_frame_buf(lcd_fb_ptr);

	list_for_each(pos,&(g_head_node->list))
	{
		ptr = list_entry(pos, linknode_t, list);
		display_video_jpeg_picture(ptr,lcd_fb_ptr,180,150,400,240);
		sleep(1);
	}
	
	
	

	return 1;

}
/***************主界面打印播放素材图片*****************/
int display_main_material(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
{
	struct list_head *pos;
	jpeg_linklist_t ptr;
	// clean_main_left_frame_buf(lcd_fb_ptr);

	list_for_each(pos,&(g_jpeg_node->list))
	{
		ptr = list_entry(pos, jpeg_linknode_t, list);
		if(strstr(ptr->info.file_name,"bofang") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,300,80,300,440);
		}
		if(strstr(ptr->info.file_name,"fangda") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,570,440);
		}
		if(strstr(ptr->info.file_name,"mutekai") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,490,440);
		}
		if(strstr(ptr->info.file_name,"fanhui") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,70,80,35,440);
		}
		if(strstr(ptr->info.file_name,"sound") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,52,30,475,385 );
		}
		if(strstr(ptr->info.file_name,"kaisuo") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,110,440 );
		}
	}
	return 1;

}


/***************主界面打印静音图标*****************/
int display_mute_black(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
{
	struct list_head *pos;
	jpeg_linklist_t ptr;
	

	list_for_each(pos,&(g_jpeg_node->list))
	{
		ptr = list_entry(pos, jpeg_linknode_t, list);
		
		if(strstr(ptr->info.file_name,"mutekai") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,490,440);
		}
		
	}
	return 1;

}

/***************主界面打印静音图标*****************/
int display_mute_red(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
{
	struct list_head *pos;
	jpeg_linklist_t ptr;
	

	list_for_each(pos,&(g_jpeg_node->list))
	{
		ptr = list_entry(pos, jpeg_linknode_t, list);
		
		if(strstr(ptr->info.file_name,"muteguan") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,490,440);
		}
		
	}
	return 1;

}

/***************主界面打印锁屏图标*****************/
int display_lock_lock(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
{
	struct list_head *pos;
	jpeg_linklist_t ptr;
	

	list_for_each(pos,&(g_jpeg_node->list))
	{
		ptr = list_entry(pos, jpeg_linknode_t, list);
		
		if(strstr(ptr->info.file_name,"guansuo") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,110,440);
		}
		
	}
	return 1;

}

/***************主界面打印解锁图标*****************/
int display_lock_unlock(const unsigned int *lcd_fb_ptr,const jpeg_linklist_t g_jpeg_node)
{
	struct list_head *pos;
	jpeg_linklist_t ptr;
	

	list_for_each(pos,&(g_jpeg_node->list))
	{
		ptr = list_entry(pos, jpeg_linknode_t, list);
		
		if(strstr(ptr->info.file_name,"kaisuo") != NULL)
		{
			display_material_jpeg_picture(ptr,lcd_fb_ptr,80,80,110,440);
		}
		
	}
	return 1;

}
/*************************************************
函数名称:   get_name   
功能: 获得路径名称中，最后一个/符合后的文件名称
被本函数调用的函数清单:无
调用本函数的函数清单: 
					file_list.c->display_next_picture;
					file_list.c->display_prev_picture;
					file_list.c->manual_display_picture
输入参数说明，包括每个参数的作用、取值说明及参数间关系：name：路径名称
输出参数: 无
函数返回值: 文件名称
其他说明: 无/
/*************************************************/
char *get_name(char *name)
{
	 char *tmp = name;
 	while(strchr(tmp,'/') != NULL)
 	{
    	tmp = strchr(tmp,'/')+1;
 	}

 	return tmp;
}
/*************************************************
函数名称:   print_char   
功能: 根据输入的文本，将之打印在lcd上
被本函数调用的函数清单:无
调用本函数的函数清单: 
					file_list.c->display_next_picture;
					file_list.c->display_prev_picture;
					file_list.c->manual_display_picture
输入参数说明，包括每个参数的作用、取值说明及参数间关系：name：路径名称 lcd_fb_ptr：lcd内存映射地址
输出参数: 无
函数返回值: 无
其他说明: 无
*************************************************/
void print_char(char *name,const unsigned int *lcd_fb_ptr,unsigned int x,unsigned int y)
{
	wchar_t wtext[50];
	memset(wtext,0,sizeof(wtext));
	int font_sub;
	setlocale( 0 , "zh_CN.gb2312" );
  	mbstowcs(wtext, name , strlen(name) );	
 	for(font_sub = 0; font_sub < strlen(name); font_sub++)
 	{
 		wtext[font_sub] = (name)[font_sub];
 	}
	Lcd_Show_FreeType(wtext,18,0x00D02090,x,y,lcd_fb_ptr);	
	
}

/*************************************************
函数名称:   right_display_picture   
功能: 在lcd右边打印3张图片
被本函数调用的函数清单:
					display_jpeg.c->display_video_jpeg_picture(ptr,lcd_fb_ptr,160,120,709,90);
					get_name(ptr->info.file_name);
					print_char(name,lcd_fb_ptr)
调用本函数的函数清单: 
					main()
输入参数说明，包括每个参数的作用、取值说明及参数间关系： lcd_fb_ptr：lcd内存映射地址 head_node:指向包含文件信息的链表结构体的头指针
输出参数: 无
函数返回值: 无
其他说明: 无
*************************************************/
int right_display_picture(const unsigned int *lcd_fb_ptr,struct list_head *now_pos,const linklist_t g_head_node)
{
	struct list_head *pos;
	linklist_t ptr;
	char *name;
	clean_main_right_frame_buf(lcd_fb_ptr);
	/**************打印第一张图片***************/
	pos = now_pos;
	if(pos == (&(g_head_node->list)) )
	{
		pos = (&(g_head_node->list))->next;
	}
	ptr = list_entry(pos, linknode_t, list);

	display_video_jpeg_picture(ptr,lcd_fb_ptr,160,120,709,90);
	name = get_name(ptr->info.file_name);
	print_char(name,lcd_fb_ptr,709-(strlen(name)*5),165);
	/**************打印第二张图片***************/
	
	pos = pos->next;
	if(pos == (&(g_head_node->list)) )
	{
		pos = (&(g_head_node->list))->next;
	}
	ptr = list_entry(pos, linknode_t, list);

	display_video_jpeg_picture(ptr,lcd_fb_ptr,160,120,709,230);
	name = get_name(ptr->info.file_name);
	print_char(name,lcd_fb_ptr,709-(strlen(name)*5),305);

	/**************打印第三张图片***************/
	pos = pos->next;
	if(pos == (&(g_head_node->list)) )
	{
		pos = (&(g_head_node->list))->next;
	}
	ptr = list_entry(pos, linknode_t, list);

	display_video_jpeg_picture(ptr,lcd_fb_ptr,160,120,709,370);
	name = get_name(ptr->info.file_name);
	print_char(name,lcd_fb_ptr,709-(strlen(name)*5),445);

	return 1;
}
