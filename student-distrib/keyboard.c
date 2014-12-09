#include "keyboard.h"
#include "i8259.h"
#include "buffer.h"
#include "terminal.h"
#include "systemcalls.h"
#include "wrapper.h"

/* 
 * KBDUS means US Keyboard Layout. This is the basic scancode table
 * used to layout a standard US keyboard. 
*/
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

/* 
 * KBDUS means US Keyboard Layout. This is the special character
 * scancode table used to layout a standard US keyboard. 
*/
unsigned char kbdus_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '\"', '~',   0,		/* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

/*
 * Declaration of buffer and global variables to be used by the functions following.
*/

int reset_flag = 0;						//checks whether the screen/buffer needs to be cleared

int caps_count = 0;						//checks whether the caps lock is on
int shift = 0;							//checks whether the shift key has been pressed
int ctrl = 0;							//checks whether the ctrl key has been pressed
int alt = 0;							//checks whether the alt key has been pressed
int term2 = 1;
int term3 = 1;

//flags and counters used to determine the position of character output on the screen
int offset = 0;
int line_flag = 1;
int limit = 0;
int line_count = 0;
volatile int enter_press = 0;


// char* args;
// int space_seen = 0;
// int index = 0;

/* 
 * kbd_int_handler()
 *   DESCRIPTION: This function is called when a keyboard interrupt is generated.
 *				  It is responsible to call out the required functions with the
 *				  necessary inputs.
 *   INPUTS: -- 
 *   OUTPUTS: --
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void kbd_int_handler()
{
	
	node* buffer = pass_buff();
	
	if(reset_flag == 0)
		disable_rtc_test();

	reset_flag = 1;
	int to_print;										//disable line so we can complete this before handling some other interrupt 	
	to_print = inb(0x60);
	kbd_logic(to_print, buffer);
	buffer = pass_buff();
	status_bar();
	if(to_print == 0x1C)
		enter_press = 1;
	else enter_press = 0;
	printb(buffer);
	
	send_eoi(1);
}

/* 
 * kbd_logic()
 *   DESCRIPTION: This function is responsible for all the keyboard operations.
 *				  Basis the input from the keyboard, this function decides the
 *				  appropriate character to be printed. It also decides the location
 *				  on the screen where this character needs to be printed.
 *   INPUTS: the integer signal received from the keyboard interrupt. 
 *   OUTPUTS: --
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets up the buffer that is to be printed to the screen.
 */
void kbd_logic(int to_print, node* buffer)
{
	//shift enable - sets flag if shift is currently pressed.
	if(to_print == 0x2A || to_print == 0x36)
		shift = 1;

	//shift disabled - clears flag if shift has been released.
	if(to_print == 0xAA || to_print == 0xB6)
		shift = 0;

	//caps_lock - sets or clears flag if caps lock is on or off
	if(to_print == 0x3A)
	{
		caps_count++;
		if(caps_count%2 == 1)
			offset = 32;
		else offset = 0;
	}	
	
	//control enable - sets flag if ctrl is currently pressed.
	if(to_print == 0x1D)
		ctrl = 1;

	//control disabled - clears flag if ctrl has been released.
	if(to_print == 0x9D)		
		ctrl = 0;

	//alt enable - sets flag if ctrl is currently pressed.
	if(to_print == 0x38)
		alt = 1;

	//alt disabled - clears flag if ctrl has been released.
	if(to_print == 0xB8)		
		alt = 0;

	//ctrl + L - reset screen if ctrl + L is hit
	if(ctrl && (to_print == 0x26))
	{
		reset_buf(buffer);
		limit = 0;
		line_count = 0;
		goto done;
	}

	if(alt && to_print == 0x3E)
		context_switch(0);

	if(alt && to_print == 0x3F)
		context_switch(1);

	
	if(alt && (to_print == 0x3B || to_print == 0x3C || to_print == 0x3D))
	{
		int prev_screen_num = screen_num;
		if(to_print == 0x3C)
		{
			screen_num = 1;
			send_eoi(1);
			terminal_switch(screen_num, prev_screen_num);
			if(term2)
			{
				term2 = 0;
				initial_shell = 1;
				uint8_t fname[33] = "shell";
				execute_syscall(fname);
			}
		}
		else if(to_print == 0x3D)
		{
			screen_num = 2;

			send_eoi(1);
			terminal_switch(screen_num, prev_screen_num);
			if(term3)
			{
				term3 = 0;
				initial_shell = 1;
				uint8_t fname[33] = "shell";
				execute_syscall(fname);
			}
		}
		else
		{
			screen_num = 0;
			send_eoi(1);
			terminal_switch(screen_num, prev_screen_num);
		}
		//terminal_switch(screen_num, prev_screen_num);
		buffer = pass_buff();
		asm volatile("jmp done_typing");
	}

	//disable typing if a command has hit the 128 character limit
	if(line_count == 127)
		line_flag = 0;
	else line_flag = 1;

	//case - no shift, no caps lock
	if(!shift && line_flag)
	{			
			if((to_print != 0x0E) && (to_print != 0x1C) && !(to_print & 0x80) && (to_print != 0x2A) && (to_print != 0x36) && (to_print != 0x3A) && (to_print != 0x1D))
			{
				int printchar = kbdus[to_print];
				if(printchar >= 97 && printchar <=122)
					printchar = printchar - offset;
				setb(buffer, printchar);
			}
			//backspace
	 		else if(to_print == 0x0E)
	 		{	
	 			backspace(buffer, line_count);
	 			if(limit >= 0) limit = limit-2; else limit = 78;
	 			if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 		}
	 		//enter
	 		else if(to_print == 0x1C)
	 		{	
	 			new_line(buffer);	
				clear_buf_line(buffer);	
				limit = 0;
				line_count = 0;
			}
			//default
	 		else
	 		{
				if(limit >= 0) limit--;
				else limit = 78;
				if(line_count >= 0) line_count--; else limit = -1;
			}
	}

	//case - shift, no caps lock
	else if(shift && line_flag)
	{
		
		
		if((to_print != 0x0E) && (to_print != 0x1C) && !(to_print & 0x80) && (to_print != 0x2A) && (to_print != 0x36) && (to_print != 0x3A) && (to_print != 0x1D))
		{
			int printchar = kbdus_shift[to_print];
			if(printchar >= 65 && printchar <=90)
				printchar = printchar + offset;
			setb(buffer, printchar);
		}
		//backspace
	 	else if(to_print == 0x0E)
	 	{	
	 		backspace(buffer, line_count);
	 		if(limit >= 0) limit = limit-2; else limit = 78;
	 		if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 	}
	 	//enter
	 	else if(to_print == 0x1C)
	 	{	
	 		new_line(buffer);	
			clear_buf_line(buffer);	
			limit = 0;
			line_count = 0;
		}
		//default
	 	else
	 	{
			if(limit >= 0) limit--;
			else limit = 78;
			if(line_count >= 0) line_count--; else limit = -1;
		}
	}

	//case - command line is full
	else
	{
		switch(to_print)
		{
			//backspace
			case 0x0E:	{	
	 					backspace(buffer, line_count);
	 					if(limit >= 0) limit = limit-1;
	 					if(line_count >= 0) line_count = line_count - 1;
	 					break;	
	 					}
	 		//enter
			case 0x1C:	{	
						new_line(buffer);
						clear_buf_line(buffer);	
						limit = 0;
						line_count = 0;
						break;	
						}
			default:	{
						
					}
		}
	}

	//if typing is allowed
	if(line_flag)
	{
		limit++;
		line_count++;
	}

	//case - if screen line limit is hit
	if(limit == 80)
	{
		new_line(buffer);
		clear_buf_line(buffer);
		limit = 0;
	}
	asm volatile("done_typing:\n\t");
	done:
		return;
}

/* 
 * pass_count()
 *   DESCRIPTION: Helper function. Used to provide other functions
 * 				  an access to current number of characters in the 
 * 				  current command.
 *   INPUTS: -- 
 *   OUTPUTS: the line count
 *   RETURN VALUE: int
 *   SIDE EFFECTS:
 */
int pass_count()
{
	return line_count;
}

/* 
 * dummy_int_handler()
 *   DESCRIPTION: for system calls
 *   INPUTS: -- 
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS: --
 */
void dummy_int_handler()
{
	//prints int #
	printf("DUMMY INTERRUPT HANDLER");
}
