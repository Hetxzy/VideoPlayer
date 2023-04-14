#pragma once
#ifndef __PTHREAD_H
#define	__PTHREAD_H
#include <all_header.h>


extern void *get_video_info(const void *arg);
extern void *write_command(const void *arg);
extern void *get_main_instrcution(const void *arg);
extern void *get_full_screen_instrcution(const void *arg);


#endif/*end define __PTHREAD_H*/
