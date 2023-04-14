/*************************************************
Copyright (C), 2018, XuRui
文件名: pthread.c
作者 : 许锐  Version: Date: 2018.08.17
功能描述: 用于存放对照片的操作，如如何打印，打印下一张或者上一张
// 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
其它内容的说明: 无
主要函数列表:
			void *get_main_instrcution(const void *arg) //获取触屏命令的线程
			void *write_command(const void *arg) // 向视频文件写命令的线程
			void *get_video_info(const void *arg) //从fifo获取文件信息的线程
			
修改历史记录列表: 
1. Date:
Author:
Modification:
2. ...
*************************************************/


#include <pthread.h>
/**************获取触摸屏动作****************/
void *get_main_instrcution(const void *arg)
{
	sem_t *g_main_instruction_sem;
	g_main_instruction_sem = sem_open(MAIN_SEMNAME, O_CREAT, 0777, 0);
	while(1)
	{
		pthread_mutex_lock(&g_main_instruction_m);//获得锁 保证以下代码不会被其他线程干扰
		/**********g_wFull_screen_flg 是1则获取全屏触屏命令 是0则获取主界面触屏指令************/
		while(g_wFull_screen_flg == 1) //0 非全屏 1全屏
		{   
			pthread_cond_wait(&g_main_instruction_v,&g_main_instruction_m);
		}
		g_main_instruction = get_instruction(); //获取触摸屏指令 
		sem_post(g_main_instruction_sem);
		pthread_mutex_unlock(&g_main_instruction_m);//解锁，使其他线程获得锁
		usleep(10000);
	}
	pthread_exit(NULL);
}   

/**************获取全屏时触摸屏动作****************/
void *get_full_screen_instrcution(const void *arg)
{
	sem_t *g_full_instruction_sem;
	g_full_instruction_sem = sem_open(FULL_SCREEN_SEMNAME, O_CREAT, 0777, 0);
	while(1)
	{
		pthread_mutex_lock(&g_main_instruction_m);//获得锁 保证以下代码不会被其他线程干扰
		/**********g_wFull_screen_flg 是1则获取全屏触屏命令 是0则获取主界面触屏指令************/
		printf("full screen g_wFull_screen_flg = %u\n",g_wFull_screen_flg);
		while(g_wFull_screen_flg == 0) //0 非全屏 1全屏
		{   
			pthread_cond_wait(&g_main_instruction_v,&g_main_instruction_m);
		}
		g_full_instruction = get_full_instruction(); //获取触摸屏指令 
		sem_post(g_full_instruction_sem);
		pthread_mutex_unlock(&g_main_instruction_m);//解锁，使其他线程获得锁
		usleep(10000);
	}
	pthread_exit(NULL);
}   

/**************向视频发送指令****************/
void *write_command(const void *arg)
{
	int fifo_fd;
	int i;
	/**************打开posix有名信号量*******************/
	sem_t *get_video_percent_read_sem;
	get_video_percent_read_sem = sem_open(MAIN_PERCENT_SEMNAME_READ, O_CREAT, 0777, 0);

	sem_t *get_video_percent_write_sem;
	get_video_percent_write_sem = sem_open(MAIN_PERCENT_SEMNAME_WRITE, O_CREAT, 0777, 0);
	/*************打开向视频写命令的fifo管道*************/
	fifo_fd = open(PATH_COMMAND, O_RDWR);
	if(fifo_fd == -1)
	{
		perror("open fifo error\n");
		return NULL;
	}
	/*************需要写的命令*********/
	char *video_msg_cmd[2]={"get_percent_pos\n","get_time_length\n"};
	while(1)
	{
		sem_wait(get_video_percent_write_sem);//等待得到写信号 得到才可以写命令
		pthread_mutex_lock(&g_write_command_m);//获得锁 保证以下代码不会被其他线程干扰
		/**********如果g_wWrite_command_flg 是0 则暂停写命令************/
		while(g_wWrite_command_flg < 1) // 0 暂时 1 播放
		{   
			pthread_cond_wait(&g_write_command_v,&g_write_command_m);
		}

		for(i = 0; i < (sizeof(video_msg_cmd)/sizeof(video_msg_cmd[0])); i++)  //(sizeof(video_msg_cmd)/sizeof(video_msg_cmd[0])) 指针数组中指针的个数
		{
			write(fifo_fd, video_msg_cmd[i],strlen(video_msg_cmd[i])); 
			usleep(150000);//每次写好需要延时会 保证命令准确写入 经试验 获得该时间
		}
   
		pthread_mutex_unlock(&g_write_command_m);//解锁，使其他线程获得锁
		sem_post(get_video_percent_read_sem);//使能读信号 允许读信息的线程可以读
		//usleep(1000); //解锁后加延时， 让读信息的线程的锁可以得到锁 或者现在是用posix有名信号量实现写了才读 读了才写 就不用延时了 
	}
	pthread_exit(0);
}    
/*************获取视频信息**************/
void *get_video_info(const void *arg)
{
	unsigned int  *lcd_fb_ptr = (unsigned int *)arg;
	char buf[600];
	int size;
	char *tmp;
	char *tmp_old;
	unsigned int m_rate_percent_old;
	/**************打开posix有名信号量*******************/
	sem_t *get_video_percent_read_sem;
	get_video_percent_read_sem = sem_open(MAIN_PERCENT_SEMNAME_READ, O_CREAT, 0777, 0);

	sem_t *get_video_percent_write_sem;
	get_video_percent_write_sem = sem_open(MAIN_PERCENT_SEMNAME_WRITE, O_CREAT, 0777, 0);
	/*************打开从视频读命令的fifo管道*************/
	int fifo_fd2;
	fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
	/************设置读取fifo_fd2为不阻塞，防止读不到信号就卡死***********/
	int status;
	status = fcntl(fifo_fd2,F_GETFL);
	status |= O_NONBLOCK;
	fcntl(fifo_fd2,F_SETFL,status);
	/***************************/
	sem_post(get_video_percent_write_sem);//第一次使能写信号 允许写信息的线程可以写
	while(1)
	{
		sem_wait(get_video_percent_read_sem);//等待得到读信号 得到才可以读命令
		pthread_mutex_lock(&g_write_command_m);//获得锁 保证以下代码不会被其他线程干扰

		/**********如果g_wWrite_command_flg 是0 则暂停读信息************/
		while(g_wWrite_command_flg < 1) // 0 暂时 1 播放
		{
			pthread_cond_wait(&g_write_command_v,&g_write_command_m);
		}

		bzero(buf,sizeof(buf));
		size = read(fifo_fd2,buf,sizeof(buf)); //读无名管道
		buf[size] = '\0'; //补全字符串
		tmp = buf;

		/************获取读取的信息的最后两个=号的内容**************/
 		while(strchr(tmp,'=') != NULL)
 		{
 			tmp_old = tmp;
    		tmp = strchr(tmp,'=')+1;
 		}
 		// if(atoi(tmp_old) != 0)
		g_wRate_percent = atoi(tmp_old);
		if(atoi(tmp) != 0)
		g_wVideo_time_max = atoi(tmp);
		printf("g_rate_percent = %d\n", g_wRate_percent);
		printf("g_video_time_max = %d\n", g_wVideo_time_max);

		/**************刷新进度条*************/
		if(0 == g_wFull_screen_flg) //主界面的进度条
		{
			m_rate_percent_old = g_wRate_percent;
			if((g_wRate_percent - m_rate_percent_old <10 ) && g_wRate_percent != 0)
			{
				redraw_main_rate_frame_buf(lcd_fb_ptr,g_wRate_percent*4);
			}
			
		}
		else if(1 == g_wFull_screen_flg)  //全屏的进度条
		{
			m_rate_percent_old = g_wRate_percent;
			if((g_wRate_percent - m_rate_percent_old <10 ) && g_wRate_percent != 0)
			{
				redraw_full_rate_frame_buf(lcd_fb_ptr,g_wRate_percent*6);
			}
		}
		

		pthread_mutex_unlock(&g_write_command_m);//解锁，使其他线程获得锁
		sem_post(get_video_percent_write_sem);//使能写信号 允许读信息的线程可以写
		// usleep(1000);//解锁后加延时， 让读信息的线程的锁可以得到锁 或者现在是用posix有名信号量实现写了才读 读了才写 就不用延时了 
	}
	
	pthread_exit(0);
}   