#include "buffer.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

#define NUM_COLS 80
#define NUM_ROWS 25
static int buf_x;
static int buf_y;
static int backspace_flag;

void reset_buf(node* buf)
{
	int i = 0;
	int x = 0;
	int y = 0;
	for(i = 0; i < NUM_ROWS*NUM_COLS; i++)
	{
		buf[i].x_coord = x;
		buf[i].y_coord = y;
		buf[i].mo = ' ';

		if(x != (NUM_COLS-1))
			x++;
		else
		{
			x = 0;
			y++;
		}
	}
	buf_x = 0;
	buf_y = 0;
}

void scroll_buf(node* buf)
{
	int i = 0;
	for(i = NUM_COLS; i < NUM_COLS*NUM_ROWS; i++)
	{
		if(buf[i].mo != '\n')
			buf[i - NUM_COLS].mo = buf[i].mo;
		else 
			buf[i - NUM_COLS].mo = ' ';
	}
	for(i = NUM_COLS*(NUM_ROWS-1); i < NUM_COLS*NUM_ROWS; i++)
	{
		buf[i].mo = ' ';
		buf_x = 0;
		buf_y = NUM_ROWS - 1;
	}
}

void setb(node* buf, char input)
{
	int index = NUM_COLS*(buf_y) + buf_x;
	buf[index].mo = input;
	//buf[index+1].mo = '_';
	if(buf_x == (NUM_COLS-1) )
	{
		buf_x = 0;
	}
	else buf_x++;
	backspace_flag = 1;
}

void printb(node* buf)
{
	int i = 0;
	for(i = 0; i < NUM_ROWS*NUM_COLS; i++)
	{
		print_helper(buf[i].mo, buf[i].x_coord, buf[i].y_coord);
	}
}

void new_line(node* buf)
{
	if(buf_x != NUM_COLS)
	{	
		int index = NUM_COLS*(buf_y) + buf_x;
		buf[index + 1].mo = '\n'; 
	}
	buf_x = 0;
	if(buf_y != (NUM_ROWS-1) )
		buf_y++;
	else scroll_buf(buf);
}

void clear_buf_line(node* buf)
{
	int i = 0;
	int index = 0;
	index = (NUM_COLS)*(buf_y);
	for(i = index; i < index + NUM_COLS; i++)
	{
		buf[i].mo = ' ';
	}
}

void backspace(node* buf, int line_count)
{
	if(line_count>0)
	{
		int index = NUM_COLS*(buf_y) + buf_x;
		buf[index-1].mo = ' ';
		// Updating the current position
		buf_x--;
		if(buf_x == -1)
		{
			buf_y--;
			buf_x = NUM_COLS-1;
			// First address of buffer
			if(buf_y <= 0 && buf_x <= 0)
			{
				buf_y = 0;
				buf_x = 0;
				backspace_flag = 0;
			}
		}
	}
	
}

int pass_y()
{
	return buf_y;
}

/*
void buf_main()
{
	

	//kbd_int_handler(to_print);

	
}*/
