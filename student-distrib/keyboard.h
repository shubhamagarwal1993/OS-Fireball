#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#ifndef ASM	
#include "lib.h"
#include "wrapper.h"
#include "types.h"
#include "rtc.h"
#define NUM_COLS 80
#define NUM_ROWS 25

extern node buffer[NUM_COLS*NUM_ROWS];

extern void kbd_int_handler();

void kbd_logic(int to_print);

extern node* pass_buff();

extern int pass_count();

extern void extern_print(char *input);

//extern void cout(char *input);

extern void dummy_int_handler();

//char* parse(char* input);

//void get_arg(int i, char* input);

#endif	/* ASM */

#endif 	/* KEYBOARD */

