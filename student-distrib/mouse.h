#ifndef _MOUSE_H
#define _MOUSE_H

#include "types.h"

typedef struct mouse_coords{
	int32_t x;
	int32_t y;
	int32_t data;
	int32_t prex_x;
	int32_t prev_y;
	int32_t del_x;
	int32_t del_y;
}coord_t;

extern void init_mouse();
extern void mouse_int_handler();
extern void show_cursor();
extern coord_t mouse_coords;
//typedef void click_handler)(int32_t, int32_t);

// void mouse_wait(int32_t a_type);
// void mouse_write(int32_t a_write);
// int32_t mouse_read();
// void mouse_install();

#endif /* _MOUSE_H */
