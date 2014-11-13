#ifndef _BUFFER_H
#define _BUFFER_H

#include "lib.h"

extern int buf_x;
extern int buf_y;

//extern void set_buffer(node* buffer);

extern void reset_buf(node* buf);

extern void clear_buf_line(node* buf);

extern void scroll_buf(node* buf);

extern void setb(node* buf, char input);

extern void printb(node* buf);

extern void new_line(node* buf);

extern void backspace(node* buf, int line_count);

extern int pass_y();

//extern void buf_main();

#endif /* _BUFFER_H */
