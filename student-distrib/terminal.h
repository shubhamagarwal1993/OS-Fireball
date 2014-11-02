#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"

extern void init_terminal();
extern void terminal_open();
extern char* terminal_read(node* buf, int counter);
extern int32_t terminal_write(node* buf, int counter);
extern int terminal_close();


#endif /* TERMINAL */

