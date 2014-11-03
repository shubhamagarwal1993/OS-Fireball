#include "buffer.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

#define NUM_COLS 80
#define NUM_ROWS 25
static int buf_x;
static int buf_y;
static int backspace_flag;

/* 
 * reset_buf()
 *   DESCRIPTION: Clears out the buffer. Also, maps x,y coordinates to respective buffer index.
 *   INPUTS: the buffer
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: resets the buffer
 */
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

/* 
 * scroll_buf()
 *   DESCRIPTION: Enables scrolling.
 *   INPUTS: the buffer
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: modifies the buffer
 */
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

/* 
 * set_buf()
 *   DESCRIPTION: Places character at appropriate index of the buffer
 *   INPUTS: the buffer, the input character
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: sets the buffer
 */
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

/* 
 * printb()
 *   DESCRIPTION: Prints the current buffer to the screen
 *   INPUTS: the buffer
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: prints the buffer to the screen.
 */
void printb(node* buf)
{
	int i = 0;
	for(i = 0; i < NUM_ROWS*NUM_COLS; i++)
	{
		print_helper(buf[i].mo, buf[i].x_coord, buf[i].y_coord);
	}
}

/* 
 * new_line()
 *   DESCRIPTION: Switches to the next line of the buffer
 *   INPUTS: the buffer
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: --
 */
void new_line(node* buf)
{
	if(buf_x != NUM_COLS)
	{	
		int index = NUM_COLS*(buf_y) + buf_x;
		//buf[index + 1].mo = '\n'; 
	}
	buf_x = 0;
	if(buf_y != (NUM_ROWS-1) )
		buf_y++;
	else scroll_buf(buf);
}

/* 
 * clear_buf_line()
 *   DESCRIPTION: clears out the current line before you start typing on it
 *   INPUTS: the buffer
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: modifies the buffer
 */
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

/* 
 * printb()
 *   DESCRIPTION: Clears out the previos character and allows you to type from that index
 *   INPUTS: the buffer, the current character count
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: modifies the buffer
 */
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

/* 
 * pass_count()
 *   DESCRIPTION: Helper function. Used to provide other functions
 * 				  an access to the current typing index's y-axis
 *   INPUTS: -- 
 *   OUTPUTS: the y coordinate
 *   RETURN VALUE: int
 *   SIDE EFFECTS:
 */
int pass_y()
{
	return buf_y;
}
