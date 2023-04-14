#pragma once
#ifndef  __ALL_HEADER_H
#define  __ALL_HEADER_H

#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <linux/input.h>
#include <math.h>

#include <setjmp.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>

#include <wchar.h>
#include <string.h>
#include <ft2build.h>
#include <signal.h>
#include <freetype.h>

#include <sys/sysmacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>


#include <memory.h> 
#include <iconv.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

#include <sys/sem.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <linux/fb.h>


#include <sys/mman.h>
#include FT_FREETYPE_H

#define SCREEN_WIDTH_MAX  800
#define SCREEN_HEIGHT_MAX 480
#define SCREEN_CENTER_X   400
#define SCREEN_CENTER_Y   240

#define WINDOWS_SHAPES_UP 2
#define WINDOWS_SHAPES    1

  
#define MAIN_DISP_X_SIZE 600


#define	PATH_COMMAND	   "/tmp/videoplayer/myfifo"
#define PATH_INFORMATION   "/tmp/videoplayer/myfifo2"

#define MAIN_SEMNAME "main_semname"
#define FULL_SCREEN_SEMNAME "full_screen_semname"
#define MAIN_PERCENT_SEMNAME_READ  "main_percent_semname_read"
#define MAIN_PERCENT_SEMNAME_WRITE "main_percent_semname_write"

#define MSGSIZE 64 //消息队列单个消息最大字节数
#define PROJ_PATH "." //消息队列使用当前路径产生key值
#define PROJ_ID 1 //两个进程要进行通讯 需要保持相同路径和id
#define FULL_TO_MAIN 1L

typedef struct ScmMsgbuf
{
    long mtype;
    int instruction;
}msgbuf_t;

typedef struct ScmPicture  //视频文件一帧图片结构体
{
	unsigned int picture_width;
    unsigned int picture_height;
    char * picture_buf;
}picture_info_t;

typedef  struct ScmVideo_file       //视频文件结构体
{   
    char file_name[50];    //文件名称    
    char file_type[50];   //文件类型    
    picture_info_t pictureInfo;
}videoFile_info_t;

typedef struct ScmJpeg_file//素材图片链表结构体
{
	char file_name[50];
	unsigned int picture_width;
    unsigned int picture_height;
    char * picture_buf;
}jpegFile_info_t;

typedef  struct  ScmVideo_node   //视频文件链表结点
{            
 videoFile_info_t  info;       
 struct list_head list;   
}linknode_t, *linklist_t;  

typedef  struct  ScmJpeg_node   //jpeg文件链表结点
{            
 jpegFile_info_t  info;       
 struct list_head list;   
}jpeg_linknode_t, *jpeg_linklist_t;  

enum screen_instruction {right_up,right_down,play_episode1,play_episode2,play_episode3,shotdown,start_play,pause_play,abort_play,\
						next_epi,prev_epi,sound_up,sound_down,speed_up,speed_down,mute,sound_control,rate_control,full_screen,lock,back}; //触摸屏的n种指令


linklist_t g_video_head_node;//视频文件头节点
jpeg_linklist_t g_jpeg_head_node;//素材图像文件头节点

unsigned int g_wScreen_touch_x;//触摸屏x轴坐标
unsigned int g_wRate_percent;//实时播放百分比
unsigned int g_wVideo_time_max;//实时播放视频总时长
unsigned int g_wPrint_type;//打印图片的方式
unsigned int g_wFull_screen_flg;// 0 非全屏 1全屏

enum screen_instruction g_main_instruction;//获取的滑屏指令
enum screen_instruction g_full_instruction;//获取的滑屏指令

unsigned int g_wWrite_command_flg; //！！！！！！！不能初始化，只能多重声明，不能多次实现，如果初始化了，多个。c文件包含此头文件，就变成多次实现初始化。！！！！！！
unsigned int *p_wLcd_fb_ptr;//打开lcd设备

pthread_cond_t g_write_command_v;//暂停/播放允许向mplayer发送指令条件变量
pthread_mutex_t g_write_command_m;//暂停/播放允许向mplayer发送指令标志位

pthread_cond_t g_main_instruction_v;//主界面获取条件变量
pthread_mutex_t g_main_instruction_m;//主界面获取指令标志位

struct list_head *g_now_jpeg_pos,*g_now_play_pos;//图像预览当前节点，视频播放当前节点

/***************输出错误信息  加static是为了该函数只在当前文件有用。。。*****************/
inline static PrintError(int val,const char*print_str)
{
    if(val == -1)
    {    
        printf("%s\n",print_str);
    }
}
/**************打开fifo*****************/
inline static OpenFifo(const char *path)
{
    int retval;
    if(access(path, F_OK))
    {
        retval = mkfifo(path, 0777);
        PrintError(retval,"creat fifo error\n");
        printf("create success\n");
    }
}


#endif
