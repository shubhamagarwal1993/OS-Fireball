#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"
#include "systemcalls.h"

#define NUM_COLS 80
#define NUM_ROWS 25

extern node buffer[NUM_COLS*NUM_ROWS];

extern node* terminal_open(file_t* file, const uint8_t* filename);
extern int32_t terminal_read(file_t* file, uint8_t* buf, int32_t counter);
extern int32_t terminal_write(file_t* file, const uint8_t* buf, int32_t counter);
extern int32_t terminal_close(file_t* file);
extern void test_read_write(node* buf, int key);
extern node* pass_buff();

extern driver_jt_t stdin_jt;
extern driver_jt_t stdout_jt;

#endif /* TERMINAL */
