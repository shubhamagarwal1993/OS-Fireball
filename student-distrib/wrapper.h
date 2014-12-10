/************************************************/
/*Created by Siddharth Murali, Aabhas Sharma,	*/	
/*			 James Lang, Shubham Agarwal		*/
/*	9th Dec, 2014								*/
/*												*/
/************************************************/

#ifndef _WRAPPER_H
#define _WRAPPER_H

#ifndef ASM

#include "types.h"

void kbd_wrapper();

void rtc_wrapper();

void pit_wrapper();

void mouse_wrapper();

void syscall_handler();

void jump_to_userspace(uint32_t entry_addr);

int32_t execute_syscall(const uint8_t* command);

#endif /* ASM */

#endif /* _WRAPPER_H */
