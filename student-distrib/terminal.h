#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"
#include "systemcalls.h"

#define NUM_COLS 80
#define NUM_ROWS 25

typedef struct buff_attr
 {
 	int curr_line;
 	int curr_limit;
 	int curr_x;
 	int curr_y;
 }buff_attr;

extern node buffer[NUM_COLS*NUM_ROWS];
extern node* screens[3];
extern int screen_num;

extern void terminal_init();
extern node* terminal_open(file_t* file, const uint8_t* filename);
extern int32_t terminal_read(file_t* file, uint8_t* buf, int32_t counter);
extern int32_t terminal_write(file_t* file, const uint8_t* buf, int32_t counter);
extern int32_t terminal_close(file_t* file);
extern node* pass_buff();
extern void terminal_switch(int new_screen, int old_screen);
int32_t screen_assign(int index);

extern driver_jt_t stdin_jt;
extern driver_jt_t stdout_jt;

#endif /* TERMINAL */
