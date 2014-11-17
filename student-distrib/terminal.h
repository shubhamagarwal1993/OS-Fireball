#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"

extern void terminal_open(int key);
extern char* terminal_read(node* buf, int counter);
extern int32_t terminal_write(node* buf, int counter, int key);
extern int terminal_close();
extern void test_read_write(node* buf, int key);


#endif /* TERMINAL */
