#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "buffer.h"
#include "i8259.h"

#define VIDEO 0xB8000
#define ATTRIB  0xE4

volatile int enter_flag = 0;
volatile int length = 0;
volatile int to_print;
node* screens[3] = {0,0,0};
int screen_num = 0;

static char* video_mem = (char *)VIDEO;

node buff_1[NUM_COLS*NUM_ROWS];
node buff_2[NUM_COLS*NUM_ROWS];
node buff_3[NUM_COLS*NUM_ROWS];
buff_attr components[3];

void terminal_init()
{
	//screens[screen_num] = buffer;
	if(screen_num < 2 && screen_num > -1)
		screen_num++;
	else screen_num = 0;
	screen_assign(screen_num);
	return;
}

/* 
 * terminal_open()
 *   DESCRIPTION: Opens the terminal and allows typing to it
 *   INPUTS: the integer signal received from the keyboard interrupt.
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS:
 */
node* terminal_open(file_t* file, const uint8_t* filename)
{
	return screens[screen_num];
	//terminal_write(buffer, 1);
}

/* 
 * terminal_read()
 *   DESCRIPTION: Read counter number of indexes from the buffer
 *   INPUTS: the buffer, the required counter
 *   OUTPUTS: string representing the required information that was read
 *   RETURN VALUE: string
 *   SIDE EFFECTS: --
 */
int32_t terminal_read(file_t* file, uint8_t* buf, int32_t counter)
{
	//enable interrupts
	sti();
	
	//clear the buffer
	int i = 0;
	for (i = 0; i < 20; i++)
	{
		buf[i] = '\0';
	}

	node* buffer = screens[screen_num];
	to_print = inb(0x60);
	char* output;
	char out[counter];
	int temp_buf_x = buf_x;
	int typed = line_count;

	//clear out
	for (i = 0; i < counter; ++i)
	{
		out[i] = '\0';
	}

	//wait for enter to get hit
	while(1)
	{
		if((length == 127)||(enter_press == 1))
			break;
		else typed = buf_x;
	}
	
	enter_press = 0;
	int y = pass_y();
	int index = 0;
	if(line_count >= 80)
		index = (y-2)*80;
	else index = (y-1)*80;
	if(enter_flag == 1)
		index = (y-1)*80;
	int j = 0;
	for(i = index + temp_buf_x; i < index + typed; i++, j++)
	{
		length++; 
		out[j] = buffer[i].mo;
		if(buffer[i+1].mo == '\n')
		{
			index = y - 1;
			out[i - index - temp_buf_x] = '\n';
			output = out;
			break;
		}
		
	}
	output = out;
	
	int yudodis;
	for(yudodis = 0; yudodis <= strlen(output); yudodis++)
	{
		buf[yudodis] = output[yudodis];
	}
	buf[yudodis-1] = '\0';
	if(counter > 128)
		return 128;
	else return counter;
}

/* 
 * terminal_write()
 *   DESCRIPTION: Writes to the terminal and prints when required
 *   INPUTS: the buffer, the required counter, the integer signal received from the keyboard interrupt.
 *   OUTPUTS: number of bytes written to the screen.
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: --
 */
int32_t terminal_write(file_t* file, const uint8_t* buf, int32_t counter)
{
	//to_print = inb(0x60);
	//kbd_int_handler();
	int i;
	char* print_this = (char*)buf;

	for(i = 0; i < strlen(print_this); i++)
	{
		// if(print_this[i] == '\n')
		// 	new_line(pass_buff());
		put_cout(print_this[i]);
	}
	printb(pass_buff());
	return counter;

}

/* 
 * terminal_close()
 *   DESCRIPTION: Close terminal. Return 0.
 *   INPUTS: --
 *   OUTPUTS: 0
 *   RETURN VALUE: int
 *   SIDE EFFECTS: --
 */
int32_t terminal_close(file_t* file)
{
	screen_num--;
	screens[screen_num] = 0;
	return 0;
}

/* 
 * pass_buff()
 *   DESCRIPTION: Helper function. Used to provide other functions
 * 				  an access to the buffer.
 *   INPUTS: -- 
 *   OUTPUTS: the buffer
 *   RETURN VALUE: node *
 *   SIDE EFFECTS:
 */
node* pass_buff()
{
	return screens[screen_num];
}

//----------------------------TERMINAL SWITCHING (3.5)-------------------------------

/* 
 * terminal_switch()
 *   DESCRIPTION: When switching between terminals, saves display buffer paramets for
 *				  previous terminal. Also updates system's display parameters according
 *				  to the newly opened terminal.
 *   INPUTS: screen_num for the terminal that is being closed and the terminal that is
 *			 now being open.
 *   OUTPUTS: -
 *   RETURN VALUE: -
 *   SIDE EFFECTS: -
 */
void terminal_switch(int new_screen, int old_screen)
{
	components[old_screen].curr_line = line_count;
	components[old_screen].curr_limit = limit;
	components[old_screen].curr_x = buf_x;
	components[old_screen].curr_y = buf_y;

	line_count  = components[new_screen].curr_line;
	limit  = components[new_screen].curr_limit;
	buf_x  = components[new_screen].curr_x;
	buf_y  = components[new_screen].curr_y;
	clear();
	printb(screens[new_screen]);
}

/* 
 * screen_assign()
 *   DESCRIPTION: Depending on screen_num determines which terminal is currently running.
 *   INPUTS: index (screen_num)
 *   OUTPUTS: 1 on success, -1 on failure
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: -
 */
int32_t screen_assign(int index)
{
	if(index == 0)
	{
		screens[index] = buff_1;
		reset_buf(buff_1);
		components[index].curr_line = 0;
		components[index].curr_limit = 0;
		components[index].curr_x = 0;
		components[index].curr_y = 0;
		return 1;
	}
	else if(index == 1)
	{
		screens[index] = buff_2;
		reset_buf(buff_2);
		components[index].curr_line = 0;
		components[index].curr_limit = 0;
		components[index].curr_x = 0;
		components[index].curr_y = 0;
		return 1;
	}
	else if(index == 2)
	{
		screens[index] = buff_3;
		reset_buf(buff_3);
		components[index].curr_line = 0;
		components[index].curr_limit = 0;
		components[index].curr_x = 0;
		components[index].curr_y = 0;
		return 1;
	}
	else return -1;
}

//----------------------------STATUS BAR (EXTRA CREDIT)-------------------------------

/* 
 * status_bar()
 *   DESCRIPTION: Physically sets up the status bar using video memory
 *   INPUTS: - 
 *   OUTPUTS: -
 *   RETURN VALUE: -
 *   SIDE EFFECTS: Modifies color on the entire status bar
 */
void status_bar()
{
	int32_t i;
	int32_t j = 0;
	char* status = " terminal1 terminal2 terminal3                                                  ";
	status = embed_time(status);
    for(i=(NUM_ROWS-1)*NUM_COLS; i<(NUM_ROWS)*NUM_COLS; i++, j++) {
        *(uint8_t *)(video_mem + (i << 1) + 1) = 0x90;
        *(uint8_t *)(video_mem + (i << 1)) = status[j];
    }
	switch_status();
}

/* 
 * switch_status()
 *   DESCRIPTION: Handles the coloring of the individual terminal keys on the status bar
 *   INPUTS: - 
 *   OUTPUTS: -
 *   RETURN VALUE: -
 *   SIDE EFFECTS: Modifies color on status bar in sync with currently open terminal
 */
void switch_status()
{
	int j = (NUM_ROWS-1)*NUM_COLS;
	int i = 0;
	if(screen_num == 0)
	{
		for(i=j; i<(NUM_ROWS-1)*NUM_COLS+11; i++, j++) 
        	*(uint8_t *)(video_mem + (i << 1) + 1) = 0x4E;
	}

	else if(screen_num == 1)
	{
		for(i=j+10; i<(NUM_ROWS-1)*NUM_COLS+21; i++, j++) 
        	*(uint8_t *)(video_mem + (i << 1) + 1) = 0x4E;
	}
	else if(screen_num == 2)
	{
		for(i=j+20; i<(NUM_ROWS-1)*NUM_COLS+31; i++, j++) 
        	*(uint8_t *)(video_mem + (i << 1) + 1) = 0x4E;
	}
}

/* 
 * embed_time()
 *   DESCRIPTION: Given the status bar string, obtains and embeds system time as hh:mm format
 * 				  at the appropriate location.
 *   INPUTS: the status message 
 *   OUTPUTS: the new status message with time embedded into it
 *   RETURN VALUE: char*
 *   SIDE EFFECTS: to avoid page fault, make sure time obtained is specifically in "hh:mm" format only.
 *	 NOTE: Name your function get_system_time to return time in "hh:mm" format to this function
 */
char* embed_time(char* status)
{
	int i = 0;

	int j = 4;
	char* sys_time = "11:03";
	//char* time = get_system_time();
	for(i = strlen(status) - 2; i > strlen(status) - 7; i--, j--)
	{
		status[i] = sys_time[j];
	}
	return status;
}
