#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"

#define NUM_COLS 80
#define NUM_ROWS 25

extern node buffer[NUM_COLS*NUM_ROWS];

extern node* terminal_open();
extern char* terminal_read(node* buf, int counter);
extern int32_t terminal_write(node* buf, int counter);
extern int terminal_close();
extern void test_read_write(node* buf, int key);
extern node* pass_buff();

#endif /* TERMINAL */
