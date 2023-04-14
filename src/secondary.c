/*************************************************
Copyright (C), 2018, XuRui
文件名: secondary.c
作者 : 许锐  Version: Date: 2018.08.17
功能描述: 直接服务于main程序的第二阶梯函数，包括触摸屏获取函数、主界面对触摸屏指令的操作函数
// 用于详细说明此程序文件完成的主要功能，与其他模块
// 或函数的接口，输出值、取值范围、含义及参数间的控
// 制、顺序、独立或依赖等关系
其它内容的说明: 无
主要函数列表:
			int main_interface_control(enum screen_instruction main_instruction);主界面对触摸屏指令的操作函数
			int get_instruction(void);触摸屏获取函数
			
修改历史记录列表: 
1. Date:
Author:
Modification:
2. ...
*************************************************/

#include <secondary.h>

int restart_main_interface(void)
{
	int restart_video_pid;//打开播放第一个视频的pid
	/**************在lcd右边打印预览界面**********/
	// right_display_picture(p_wLcd_fb_ptr,g_now_jpeg_pos,g_video_head_node);

	int m_iSwitchtime;
	m_iSwitchtime = g_wVideo_time_max*g_wRate_percent/100;
	char m_cSetTime[10];
	sprintf(m_cSetTime,"%d",m_iSwitchtime);
	usleep(300000);
	/********打印素材图片*******/
	display_main_material(p_wLcd_fb_ptr,g_jpeg_head_node);
	/********将进度条、声音控制条的背景纯色化******************/
	clean_main_rate_frame_buf(p_wLcd_fb_ptr);
	clean_main_sound_frame_buf(p_wLcd_fb_ptr);
	/**************在lcd右边打印预览界面**********/
	right_display_picture(p_wLcd_fb_ptr,g_now_jpeg_pos,g_video_head_node);

	restart_video_pid = fork();
	if(restart_video_pid == 0)
	{   
		int fifo_fd2;
		fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
		dup2(fifo_fd2,STDOUT_FILENO);
		
		linklist_t ptr;
		ptr = list_entry(g_now_play_pos, linknode_t, list);
		execlp("mplayer","mplayer","-slave","-quiet","-ss",m_cSetTime,"-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
		exit(0);
	}
	waitpid(restart_video_pid,NULL,WNOHANG);
	
	printf("display material picture\n");
	
}


int main_interface_control(enum screen_instruction main_instruction)
{
	static int mute_cnt = 0;
	static int lock_cnt = 0;
	int breakFlg = 1;
	char sound_cont[20];
	unsigned int sound_control_cnt,rate_control_cnt;//声音控制百分比，进度条控制百分比
	pid_t full_screen_id;
	/***************打开获得触屏指令的POSIX有名信号量********************/
	sem_t *g_main_instruction_sem;
	g_main_instruction_sem = sem_open(MAIN_SEMNAME, O_CREAT, 0777, 0);

	/*************打开写命令的FIFO******************/
	int fifo_command_fd = open(PATH_COMMAND, O_RDWR);
	PrintError(fifo_command_fd,"open fifo error");
	/*************获取信息队列*******************/
	key_t full_to_main_key = ftok(PROJ_PATH, PROJ_ID);
	int full_to_main_msgid = msgget(full_to_main_key, IPC_CREAT | 0666);

	msgbuf_t msg_rcv_buf;
	bzero(&msg_rcv_buf, sizeof(msg_rcv_buf));
	/*************触屏指令操作*******************/
	switch (main_instruction)  
		{
			/*************上滑，查看下一集截图***************/
			case right_up:
				printf("right_up\n"); 
				g_wPrint_type = WINDOWS_SHAPES_UP;
				g_now_jpeg_pos = g_now_jpeg_pos->next;
				if(g_now_jpeg_pos == (&(g_video_head_node->list)) )
				{
					g_now_jpeg_pos = (&(g_video_head_node->list))->next;
				}
				right_display_picture(p_wLcd_fb_ptr,g_now_jpeg_pos,g_video_head_node);
				break;
			/*************下滑，查看上一集截图***************/
			case right_down:
				printf("right_down\n");
				g_wPrint_type = WINDOWS_SHAPES;   
				g_now_jpeg_pos = g_now_jpeg_pos->prev;
				if(g_now_jpeg_pos == (&(g_video_head_node->list)) )
				{
					g_now_jpeg_pos = (&(g_video_head_node->list))->prev; 
				}
				right_display_picture(p_wLcd_fb_ptr,g_now_jpeg_pos,g_video_head_node);	
				break;
			/************播放第一集****************/
			case play_episode1:
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_now_play_pos = g_now_jpeg_pos;
				pid_t epi_1_pid;//进程id
				epi_1_pid = fork();
				if(epi_1_pid == 0)
				{
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,0);
					execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_1_pid,NULL,WNOHANG);
				break;
			/************播放第二集****************/
			case play_episode2:
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_now_play_pos =  g_now_jpeg_pos->next;
				if(g_now_play_pos == (&(g_video_head_node->list)) )
					{
						g_now_play_pos = (&(g_video_head_node->list))->next;
					}	
				pid_t epi_2_pid;
				epi_2_pid = fork();
				if(epi_2_pid == 0)
				{  
					   
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,0);
					execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_2_pid,NULL,WNOHANG);
				break;
			/***********播放第三集*****************/
			case play_episode3:
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_now_play_pos = g_now_jpeg_pos->next;
					if(g_now_play_pos == (&(g_video_head_node->list)) )
					{
						g_now_play_pos = (&(g_video_head_node->list))->next;
					}	
					g_now_play_pos = g_now_play_pos->next;
					if(g_now_play_pos == (&(g_video_head_node->list)) )
					{
						g_now_play_pos = (&(g_video_head_node->list))->next;
					}	
				pid_t epi_3_pid;
				epi_3_pid = fork();
				if(epi_3_pid == 0)
				{
				
					
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,0);
					execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_3_pid,NULL,WNOHANG);
				break;
			/*************终止播放***************/
			case abort_play:
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				break;
			/*************开始***************/
			case start_play:
				pthread_mutex_lock(&g_write_command_m);
				g_wWrite_command_flg = 1;
				pthread_cond_broadcast(&g_write_command_v);
				pthread_mutex_unlock(&g_write_command_m);
				write(fifo_command_fd, "pause\n",strlen("pause\n"));
				break;
			/*************暂停***************/
			case pause_play:
				pthread_mutex_lock(&g_write_command_m);
				g_wWrite_command_flg = 0;
				pthread_mutex_unlock(&g_write_command_m);
				write(fifo_command_fd, "pause\n",strlen("pause\n"));
				break;
			/*************增强5度声音***************/
			case sound_up:
				write(fifo_command_fd,"volume +5\n",strlen("volume +5\n"));
				break;
			/************降低5度声音****************/
			case sound_down:
				write(fifo_command_fd,"volume -5\n",strlen("volume -5\n"));
				break;
			/***********前进5秒*****************/
			case speed_up:
				write(fifo_command_fd,"seek +5\n",strlen("seek +5\n"));
				break;
			/***********后退5秒*****************/
			case speed_down:
				write(fifo_command_fd,"seek -5\n",strlen("seek -5\n"));
				break;
			/************声音控制****************/
			case sound_control:
				sound_control_cnt = g_wScreen_touch_x - 500;
				sprintf(sound_cont,"volume %u 1\n",sound_control_cnt);
				write(fifo_command_fd,sound_cont,strlen(sound_cont));
				redraw_main_sound_frame_buf(p_wLcd_fb_ptr,sound_control_cnt);
				break;
			/************进度条控制****************/
			case rate_control:
				rate_control_cnt = (g_wScreen_touch_x - 25)/4;
				printf("rate_control_cnt = %u\n", rate_control_cnt);
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_wRate_percent = rate_control_cnt;
				pid_t epi_rate_control_pid;
				int time;
				time = g_wVideo_time_max*rate_control_cnt/100;
				char setTime[10];
				sprintf(setTime,"%d",time);
				epi_rate_control_pid = fork();
				if(epi_rate_control_pid == 0)
				{
					
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,rate_control_cnt*4);
					execlp("mplayer","mplayer","-slave","-ss",setTime,"-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_rate_control_pid,NULL,WNOHANG);;
				break;
			/*************下一集***************/
			case next_epi:
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_now_play_pos =  g_now_play_pos->next;
				if(g_now_play_pos == (&(g_video_head_node->list)) )
					{
						g_now_play_pos = (&(g_video_head_node->list))->next;
					}
				pid_t epi_next_pid;
				epi_next_pid = fork();
				if(epi_next_pid == 0)
				{
					
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,0);
					execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_next_pid,NULL,WNOHANG);
				break;
			/*************前一集***************/
			case prev_epi:
				mute_cnt = 0;
				display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				write(fifo_command_fd, "quit\n",strlen("quit\n"));
				usleep(100000);
				g_now_play_pos =  g_now_play_pos->prev;
				if(g_now_play_pos == (&(g_video_head_node->list)) )
					{
						g_now_play_pos = (&(g_video_head_node->list))->prev;
					}
				pid_t epi_prev_pid;
				epi_prev_pid = fork();
				if(epi_prev_pid == 0)
				{
					   
					int fifo_fd2;
					fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
					dup2(fifo_fd2,STDOUT_FILENO);
					linklist_t ptr;
					
					ptr = list_entry(g_now_play_pos, linknode_t, list);
					
					redraw_main_rate_frame_buf(p_wLcd_fb_ptr,0);
					execlp("mplayer","mplayer","-slave","-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","610","-y","370",ptr->info.file_name,NULL);
					exit(0);
				}
				waitpid(epi_prev_pid,NULL,WNOHANG);
				break;
			/*************全屏***************/
			case full_screen:
				g_wFull_screen_flg = 1; //设置为全屏模式
				

				full_screen_id = fork();
				if(full_screen_id == 0)
				{
					int breakFlg = 1;
					pid_t full_screen_play_id;
					write(fifo_command_fd, "quit\n",strlen("quit\n"));
					usleep(100000);

					sem_t *g_full_instruction_sem;
					g_full_instruction_sem = sem_open(FULL_SCREEN_SEMNAME, O_CREAT, 0777, 0);

					int m_iSwitchtime;
					m_iSwitchtime = g_wVideo_time_max*g_wRate_percent/100;
					char m_cSetTime[10];
					sprintf(m_cSetTime,"%d",m_iSwitchtime);

					/********将进度条、声音控制条的背景纯色化******************/
					clean_full_rate_frame_buf(p_wLcd_fb_ptr);
					clean_full_sound_frame_buf(p_wLcd_fb_ptr);
					/*************获取信息队列*******************/
					key_t full_to_main_key = ftok(PROJ_PATH, PROJ_ID);
					int full_to_main_msgid = msgget(full_to_main_key, IPC_CREAT | 0666);

					msgbuf_t msg_snd_buf;
					bzero(&msg_snd_buf, sizeof(msg_snd_buf));
					msg_snd_buf.mtype = FULL_TO_MAIN;
					/************子进程里再开子进程进行全屏播放*************/
					full_screen_play_id = fork();
					if(full_screen_play_id == 0)
					{
						int fifo_fd2;
						fifo_fd2 = open(PATH_INFORMATION, O_RDWR);
						dup2(fifo_fd2,STDOUT_FILENO);
						linklist_t ptr;
					
						ptr = list_entry(g_now_play_pos, linknode_t, list);
						redraw_main_rate_frame_buf(p_wLcd_fb_ptr,rate_control_cnt*4);
						execlp("mplayer","mplayer","-slave","-ss",m_cSetTime,"-quiet","-input","file=/tmp/videoplayer/myfifo","-geometry",\
							"0:0","-zoom","-x","800","-y","450",ptr->info.file_name,NULL);
						exit(0);
					}
					waitpid(full_screen_play_id,NULL,WNOHANG);
					/***********开启全屏模式下的触摸屏查询线程**************/
					pthread_t get_full_instrcution_tid;
					pthread_create(&get_full_instrcution_tid, NULL, get_full_screen_instrcution, NULL);
					pthread_cond_broadcast(&g_main_instruction_v);
					/***********开始全屏时操作触摸屏*************/
					while(breakFlg)
					{
						printf("here is full screen while\n");
						/*****等待有触屏操作，防止重复执行上一个命令，进入死循环********/
						sem_wait(g_full_instruction_sem);
						msg_snd_buf.instruction = g_full_instruction;
						if(msgsnd(full_to_main_msgid, &msg_snd_buf, sizeof(msg_snd_buf.instruction), 0) != 0)
						{
							perror("msgsnd() error");
							exit(1);
						}
						/************按触屏指令进行操作****************/
						switch(g_full_instruction)
						{
							case back:
								write(fifo_command_fd, "quit\n",strlen("quit\n"));
								usleep(10000);
								restart_main_interface();
								// usleep(100000);
								breakFlg = 0;
								break;
							default:
								break;
						}
					}
					/*********即将退出全屏子进程，取消并结合全屏模式下查询子进程的线程**************/
					pthread_cancel(get_full_instrcution_tid);
					pthread_join(get_full_instrcution_tid,NULL);
					exit(0);
				}//全屏子进程结束
				/***********获取全屏子进程的触摸屏指令，对父进程相应的参数操作**************************/
				while(breakFlg)
				{
					bzero(&msg_rcv_buf,sizeof(msg_rcv_buf));
					if(msgrcv(full_to_main_msgid, &msg_rcv_buf, MSGSIZE, FULL_TO_MAIN, 0) == -1)
					{
						perror("msgrcv() error");
						exit(1);
					}
					printf("msg_rcv_buf.instruction = %d\n", msg_rcv_buf.instruction);
					if(msg_rcv_buf.instruction == pause_play)
					{
						pthread_mutex_lock(&g_write_command_m);
						g_wWrite_command_flg = 0;
						pthread_mutex_unlock(&g_write_command_m);
						write(fifo_command_fd, "pause\n",strlen("pause\n"));
					}
					if(msg_rcv_buf.instruction == start_play)
					{
						pthread_mutex_lock(&g_write_command_m);
						g_wWrite_command_flg = 1;
						pthread_cond_broadcast(&g_write_command_v);
						pthread_mutex_unlock(&g_write_command_m);
						write(fifo_command_fd, "pause\n",strlen("pause\n"));
					}
					if(msg_rcv_buf.instruction == back)
					{
						breakFlg = 0;
					}
				}
				
				/***********子进程结束************/
				waitpid(full_screen_id,NULL,0);
				printf("child exit\n");
				/***********退出全屏，标志位复位，并唤醒主界面的获取触摸屏指令线程************/
				g_wFull_screen_flg = 0;
				pthread_cond_broadcast(&g_main_instruction_v);
				usleep(10000);
				/***********重新刷新主界面**********************/
				// restart_main_interface();
				break;
			/*************静音***************/
			case mute:
				mute_cnt++;
				if(mute_cnt%2 == 1)//按下奇数次
				{
					write(fifo_command_fd, "mute 1\n",strlen("mute 1\n"));
					display_mute_red(p_wLcd_fb_ptr,g_jpeg_head_node);
				}
				
				if(mute_cnt%2 == 0)//按下偶数次
				{
					write(fifo_command_fd, "mute 0\n",strlen("mute 0\n"));
					display_mute_black(p_wLcd_fb_ptr,g_jpeg_head_node);
				}
				break;
			/***********锁屏*****************/
			case lock:
					display_lock_lock(p_wLcd_fb_ptr,g_jpeg_head_node);
					sem_wait(g_main_instruction_sem);
					while(g_main_instruction != lock);
					display_lock_unlock(p_wLcd_fb_ptr,g_jpeg_head_node);
				break;	
			default :
				break;
		}
}

/*************************************************
函数名称:   get_instruction   
功能: 获得触摸屏的指令
被本函数调用的函数清单: 
					list_for_all_file(name_buf,head_node);
调用本函数的函数清单: 
					main()
输入参数说明，包括每个参数的作用、取值说明及参数间关系：无
输出参数: 无
函数返回值: prev_picture: 上一张图片 next_picture： 下一张图片 zoom_out ：上一张图片 zoom_up： 下一张图片
其他说明: 无
*************************************************/
int get_instruction(void)
{
	// １，打开触摸屏设备
	int screen_touch_fd = open("/dev/input/event0", O_RDONLY);
	// ２，读取手指坐标
	struct input_event screen_touch_buf;
	unsigned int x_start = 0,x_end = 0,x_cnt = 0;
	unsigned int y_start = 0,y_end = 0,y_cnt = 0;
	while(1)
	{
		bzero(&screen_touch_buf, sizeof(screen_touch_buf));
		read(screen_touch_fd, &screen_touch_buf, sizeof(screen_touch_buf));

		// 检测到触摸屏的坐标信息
		if(screen_touch_buf.type == EV_ABS)
		{
			
			if(screen_touch_buf.code == ABS_X )
			{
				x_cnt++;
				if(x_cnt == 1)
				{
					x_start = screen_touch_buf.value;
				}
				printf("(%d,", screen_touch_buf.value);
				x_end = screen_touch_buf.value;
				
			}
			if(screen_touch_buf.code == ABS_Y )
			{
				y_cnt++;
				if(y_cnt == 1)
				{
					y_start = screen_touch_buf.value;
				}
				printf("%d)\n", screen_touch_buf.value);
				y_end = screen_touch_buf.value;	
				
			}
			
		}

		// 检测到按键信息
		if(screen_touch_buf.type == EV_KEY)
		{
			// 检测到触摸屏的按压信息
			if(screen_touch_buf.code == BTN_TOUCH)
			{
				// 检测到松开了触摸屏
				if(screen_touch_buf.value == 0)
				{
					break;	
				}
			}
		}
	}

	close(screen_touch_fd);

	g_wScreen_touch_x = x_start;
	
	if(x_end == x_start && y_end ==y_start) //单点
	{
		 if (x_start > 610 && (y_start > 30 && y_start < 170) )
			return play_episode1;//单点触摸屏幕，播放第一个视频

		else if (x_start > 610 && (y_start > 170 && y_start < 290) )
			return play_episode2;//单点触摸屏幕，播放第二个视频

		else if (x_start > 610 && (y_start > 310 && y_start < 430) )
			return play_episode3;//单点触摸屏幕，播放第三个视频

		else if (x_start < 40 && (y_start < 40) )
			return shotdown;//单点触摸屏幕，播放第三个视频

		else if (x_start < 320 && x_start >280 && y_start < 480 && y_start > 400)
			return start_play;//单点触摸屏幕，开始播放视频

		else if (x_start < 265 && x_start >225 && y_start < 480 && y_start > 400)
			return abort_play;//单点触摸屏幕，终止播放视频

		else if (x_start < 210 && x_start >170 && y_start < 480 && y_start > 400)
			return prev_epi;//单点触摸屏幕，播放上一个视频

		else if (x_start < 375 && x_start >335 && y_start < 480 && y_start > 400)
			return pause_play;//单点触摸屏幕，暂停播放视频

		else if (x_start < 430 && x_start >390 && y_start < 480 && y_start > 400)
			return next_epi;//单点触摸屏幕，播放下一个视频

		else if (x_start < 530 && x_start >450 && y_start < 480 && y_start > 400)
			return mute;//单点触摸屏幕，静音

		else if (x_start < 610 && x_start >530 && y_start < 480 && y_start > 400)
			return full_screen;//单点触摸屏幕，静音

		else if (x_start < 600 && x_start >500 && y_start < 400 && y_start > 370)
			return sound_control;//单点触摸屏幕，静音

		else if (x_start < 425 && x_start >25 && y_start < 400 && y_start > 370)
			return rate_control;//单点触摸屏幕，改进度条

		else if (x_start < 150 && x_start >70 && y_start < 480 && y_start > 400)
			return lock;//单点触摸屏幕，静音
	}
	
	else if(x_start > 640 && (y_start > 30 && y_start < 150) && x_end > 640 && (y_end > 30 && y_end < 150))//单点触摸屏幕，防止抖动，播放第一个视频
	{
		return play_episode1;
	}
	else if(x_start > 640 && (y_start > 170 && y_start < 290)  && x_end > 640 && (y_end > 170 && y_end < 290))//单点触摸屏幕，防止抖动，播放第二个视频
	{
		return play_episode2;
	}
	else if(x_start > 640 && (y_start > 310 && y_start < 430) && x_end > 640 && (y_end > 310 && y_end < 490))//单点触摸屏幕，防止抖动，播放第三个视频
	{
		return play_episode3;
	}
	else if(y_end < y_start && (y_start - y_end > 20) && (abs(x_start - x_end) < 100)&& x_start > 610 && x_end > 610)//上划，切换下一个视频
	{
		return right_up;
	}
	else if(y_end > y_start && (y_end - y_start > 20) && (abs(x_start - x_end) < 100)&& x_start > 610 && x_end > 610)//下划，切换上一个视频
	{
		return right_down;
	}
	else if(y_end > y_start && (y_end - y_start > 20) && (abs(x_start - x_end) < 100)&& x_start < 200 && x_end < 200)//下划，降低视频声音
	{
		return sound_down;
	}
	else if(y_end < y_start && (y_start - y_end > 20) && (abs(x_start - x_end) < 100)&& x_start < 200 && x_end < 200)//上划，增大视频声音
	{
		return sound_up;
	}
	else if(x_end < x_start && (x_start - x_end > 20) && (abs(y_start - y_end) < 100)&& y_start < 370 && y_end < 370)//左划，后退5秒
	{
		return speed_down;
	}
	else if(x_end > x_start && (x_end - x_start > 20) && (abs(y_start - y_end) < 100)&& y_start < 370 && y_end < 370)//右划，前进5秒
	{
		return speed_up;
	}
	else
	{
		return -1;
	}
	
}


/*************************************************
函数名称:   get_instruction   
功能: 获得触摸屏的指令
被本函数调用的函数清单: 
					list_for_all_file(name_buf,head_node);
调用本函数的函数清单: 
					main()
输入参数说明，包括每个参数的作用、取值说明及参数间关系：无
输出参数: 无
函数返回值: prev_picture: 上一张图片 next_picture： 下一张图片 zoom_out ：上一张图片 zoom_up： 下一张图片
其他说明: 无
*************************************************/
int get_full_instruction(void)
{
	// １，打开触摸屏设备
	int screen_touch_fd = open("/dev/input/event0", O_RDONLY);
	// ２，读取手指坐标
	struct input_event screen_touch_buf;
	static int play_flg = 0;
	unsigned int x_start = 0,x_end = 0,x_cnt = 0;
	unsigned int y_start = 0,y_end = 0,y_cnt = 0;
	while(1)
	{
		bzero(&screen_touch_buf, sizeof(screen_touch_buf));
		read(screen_touch_fd, &screen_touch_buf, sizeof(screen_touch_buf));

		// 检测到触摸屏的坐标信息
		if(screen_touch_buf.type == EV_ABS)
		{
			
			if(screen_touch_buf.code == ABS_X )
			{
				x_cnt++;
				if(x_cnt == 1)
				{
					x_start = screen_touch_buf.value;
				}
				printf("(%d,", screen_touch_buf.value);
				x_end = screen_touch_buf.value;
				
			}
			if(screen_touch_buf.code == ABS_Y )
			{
				y_cnt++;
				if(y_cnt == 1)
				{
					y_start = screen_touch_buf.value;
				}
				printf("%d)\n", screen_touch_buf.value);
				y_end = screen_touch_buf.value;	
				
			}
			
		}

		// 检测到按键信息
		if(screen_touch_buf.type == EV_KEY)
		{
			// 检测到触摸屏的按压信息
			if(screen_touch_buf.code == BTN_TOUCH)
			{
				// 检测到松开了触摸屏
				if(screen_touch_buf.value == 0)
				{
					break;	
				}
			}
		}
	}

	close(screen_touch_fd);

	g_wScreen_touch_x = x_start;
	
	if(x_end == x_start && y_end ==y_start) //单点
	{
		 if (x_start > 610 && (y_start > 30 && y_start < 170) )
			return play_episode1;//单点触摸屏幕，播放第一个视频

		else if (x_start < 50 && x_start >0 && y_start < 50 && y_start > 0)
			return back;//单点触摸屏幕，开始播放视频
		else if (x_start < 799 && x_start >0 && y_start < 450 && y_start > 50)
		{
			play_flg++;
			if(play_flg % 2 == 1)
			{
				return pause_play;
			}
			if(play_flg % 2 == 0)
			{
				return start_play;
			}
		}
		/*else if (x_start > 610 && (y_start > 170 && y_start < 290) )
			return play_episode2;//单点触摸屏幕，播放第二个视频

		else if (x_start > 610 && (y_start > 310 && y_start < 430) )
			return play_episode3;//单点触摸屏幕，播放第三个视频

		else if (x_start < 40 && (y_start < 40) )
			return shotdown;//单点触摸屏幕，播放第三个视频

		else if (x_start < 320 && x_start >280 && y_start < 480 && y_start > 400)
			return start_play;//单点触摸屏幕，开始播放视频

		else if (x_start < 265 && x_start >225 && y_start < 480 && y_start > 400)
			return abort_play;//单点触摸屏幕，终止播放视频

		else if (x_start < 210 && x_start >170 && y_start < 480 && y_start > 400)
			return prev_epi;//单点触摸屏幕，播放上一个视频

		else if (x_start < 375 && x_start >335 && y_start < 480 && y_start > 400)
			return pause_play;//单点触摸屏幕，暂停播放视频

		else if (x_start < 430 && x_start >390 && y_start < 480 && y_start > 400)
			return next_epi;//单点触摸屏幕，播放下一个视频

		else if (x_start < 530 && x_start >450 && y_start < 480 && y_start > 400)
			return mute;//单点触摸屏幕，静音

		else if (x_start < 610 && x_start >530 && y_start < 480 && y_start > 400)
			return full_screen;//单点触摸屏幕，静音

		else if (x_start < 600 && x_start >500 && y_start < 400 && y_start > 370)
			return sound_control;//单点触摸屏幕，静音

		else if (x_start < 425 && x_start >25 && y_start < 400 && y_start > 370)
			return rate_control;//单点触摸屏幕，改进度条

		else if (x_start < 150 && x_start >70 && y_start < 480 && y_start > 400)
			return lock;//单点触摸屏幕，静音*/
	}
	/*
	else if(x_start > 640 && (y_start > 30 && y_start < 150) && x_end > 640 && (y_end > 30 && y_end < 150))//单点触摸屏幕，防止抖动，播放第一个视频
	{
		return play_episode1;
	}
	else if(x_start > 640 && (y_start > 170 && y_start < 290)  && x_end > 640 && (y_end > 170 && y_end < 290))//单点触摸屏幕，防止抖动，播放第二个视频
	{
		return play_episode2;
	}
	else if(x_start > 640 && (y_start > 310 && y_start < 430) && x_end > 640 && (y_end > 310 && y_end < 490))//单点触摸屏幕，防止抖动，播放第三个视频
	{
		return play_episode3;
	}
	else if(y_end < y_start && (y_start - y_end > 20) && (abs(x_start - x_end) < 100)&& x_start > 610 && x_end > 610)//上划，切换下一个视频
	{
		return right_up;
	}
	else if(y_end > y_start && (y_end - y_start > 20) && (abs(x_start - x_end) < 100)&& x_start > 610 && x_end > 610)//下划，切换上一个视频
	{
		return right_down;
	}
	else if(y_end > y_start && (y_end - y_start > 20) && (abs(x_start - x_end) < 100)&& x_start < 200 && x_end < 200)//下划，降低视频声音
	{
		return sound_down;
	}
	else if(y_end < y_start && (y_start - y_end > 20) && (abs(x_start - x_end) < 100)&& x_start < 200 && x_end < 200)//上划，增大视频声音
	{
		return sound_up;
	}
	else if(x_end < x_start && (x_start - x_end > 20) && (abs(y_start - y_end) < 100)&& y_start < 370 && y_end < 370)//左划，后退5秒
	{
		return speed_down;
	}
	else if(x_end > x_start && (x_end - x_start > 20) && (abs(y_start - y_end) < 100)&& y_start < 370 && y_end < 370)//右划，前进5秒
	{
		return speed_up;
	}*/
	else
	{
		return -1;
	}
	
}