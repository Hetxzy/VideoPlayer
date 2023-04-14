#pragma once
#ifndef __SECONDARY_H
#define	__SECONDARY_H
#include <all_header.h>

extern int main_interface_control(enum screen_instruction main_instruction);
extern int get_instruction(void);
extern int restart_main_interface(void);
extern int get_full_instruction(void);

#endif/*end define __SECONDARY_H*/
