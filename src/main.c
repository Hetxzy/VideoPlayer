#include <file_list.h>
#include <lcd.h>
#include <display_jpeg.h>
#include <pthread.h>
   

int main(int argc, const char *argv[])
{
	int lcd_fd; //打开lcd设备
	
	int retval; 
	
	int first_video_pid;//打开播放第一个视频的pid
	g_wWrite_command_flg = 1;
	g_wPrint_type = 0;
	/***************初始化互斥锁和条件变量***********/
	pthread_mutex_init(&g_write_command_m,NULL);
	pthread_cond_init(&g_write_command_v,NULL);

	pthread_mutex_init(&g_main_instruction_m,NULL);
	pthread_cond_init(&g_main_instruction_v,NULL);

	/**********创建保存文件信息的链表并初始化*********/
    g_video_head_node = create_file_link_init();

    /**********遍历整个目录，将视频文件保存下来********/
	retval = list_for_all_file(argv[1],g_video_head_node); 
	PrintError(retval,"list for all file failed");

	/**********创建保存jpeg信息的链表并初始化*********/
    g_jpeg_head_node = create_jpeg_link_init();

    /**********遍历整个目录，将jepg文件保存下来*******/
	retval = list_for_all_jpeg_file("material",g_jpeg_head_node); 
	PrintError(retval,"list for all jpeg file failed");

	/**********打开lcd设备，获得地址映射地址**********/
	lcd_fd = open_lcd_device(&p_wLcd_fb_ptr);
	PrintError(lcd_fd,"open lcd failed");
    
	/****************创建有名管道*******************/
	OpenFifo(PATH_COMMAND);
	OpenFifo(PATH_INFORMATION);
	/*************打开写命令的FIFO******************/
	int fifo_command_fd = open(PATH_COMMAND, O_RDWR);
	PrintError(fifo_command_fd,"open fifo error");
  
   
	/***************打开获得触屏指令的POSIX有名信号量********************/
	sem_t *g_main_instruction_sem;
	g_main_instruction_sem = sem_open(MAIN_SEMNAME, O_CREAT, 0777, 0);
	/**************逐一显示一帧图片 检验遍历目录是否正确***************
	display_all_jpeg(p_wLcd_fb_ptr, g_video_head_node); */
 	
	/**************在lcd右边打印预览界面，获得指向当前图像的指针**********/
	g_now_jpeg_pos = (&(g_video_head_node->list))->next;
	right_display_picture(p_wLcd_fb_ptr,g_now_jpeg_pos,g_video_head_node);

	/******************创建子进程，播放第一个视频*******************/
 	g_now_play_pos = g_now_jpeg_pos;
	first_video_pid = fork();
	if(first_video_pid == 0)
	{   
		int fifo_fd2;
		fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
		dup2(fifo_fd2,STDOUT_FILENO);
		
		linklist_t ptr;
		ptr = list_entry(g_now_play_pos, linknode_t, list);
		execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
		exit(0);
	}
	waitpid(first_video_pid,NULL,WNOHANG);

	/***************打开查询触屏、写指令、读视频信息的3个线程*****************/
	pthread_t get_main_instrcution_tid;
	pthread_create(&get_main_instrcution_tid, NULL, get_main_instrcution, NULL);

	pthread_t write_command_tid;
	pthread_create(&write_command_tid, NULL, write_command, NULL);

	pthread_t get_video_info_tid;
	pthread_create(&get_video_info_tid, NULL, get_video_info, (void*)p_wLcd_fb_ptr);

	
	/********打印素材图片*******/
	display_main_material(p_wLcd_fb_ptr,g_jpeg_head_node);
	/********将进度条、声音控制条的背景纯色化******************/
	clean_main_rate_frame_buf(p_wLcd_fb_ptr);
	clean_main_sound_frame_buf(p_wLcd_fb_ptr);
	void *ret;

	/***********开始操作触摸屏*************/
	int continue_flg = 1;
	while(continue_flg&&1)
	{
		/*****等待有触屏操作，防止重复执行上一个命令，进入死循环********/
		sem_wait(g_main_instruction_sem);
		/************按触屏指令进行操作****************/
		main_interface_control(g_main_instruction);
	}

	/******************回收线程******************/
	pthread_join(get_main_instrcution_tid,NULL);
	pthread_join(write_command_tid,NULL);   
	pthread_join(get_video_info_tid,NULL);
	
	/*************关闭lcd，删除视频文件链表***********/
	close_lcd_device(lcd_fd, p_wLcd_fb_ptr,g_video_head_node);

	return 0;
}     


