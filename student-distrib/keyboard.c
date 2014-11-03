#include "keyboard.h"
#include "i8259.h"
#include "buffer.h"
#include "terminal.h"

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
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


node buffer[NUM_COLS*NUM_ROWS];
int reset_flag = 0;

int caps_count = 0;
int shift = 0;
int ctrl = 0;
int offset = 0;
int line_flag = 1;

int limit = 0;
int line_count = 0;


void kbd_int_handler()
{
	if(reset_flag == 0)
	{
		reset_scr();
		reset_buf(buffer);
		disable_rtc_test();
	}
	reset_flag = 1;

	int to_print;										//disable line so we can complete this before handling some other interrupt 	
	to_print = inb(0x60);
	kbd_logic(to_print);
	//printb(buffer);
	//init_terminal();
	
	//test_filesys();
	terminal_open(to_print);
	//test_read_write(buffer, to_print);
	
	send_eoi(1);
}


/* Keyboard Interrupt handler */
/********************************************************
void													*
kbd_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		prints pressed button on screen 	*
*	Return Value:	void								*
*	Function: Prints out character typed to screen		*
********************************************************/
void kbd_logic(int to_print)
{
	//disable_irq(1);										//IRQ line was 1 so we get the port numbers
									//store received info into char to check with which key was pressed 
	

	if(to_print == 0x2A || to_print == 0x36)
		shift = 1;

	//shift disabled
	if(to_print == 0xAA || to_print == 0xB6)
		shift = 0;

	if(to_print == 0x3A)
	{
		caps_count++;
		if(caps_count%2 == 1)
			offset = 32;
		else offset = 0;
	}	
	
	/* control press and release */
	if(to_print == 0x1D)
		ctrl = 1;

	if(to_print == 0x9D)		
		ctrl = 0;


	if(ctrl && (to_print == 0x26))
	{
		reset_buf(buffer);
		limit = 0;
		line_count = 0;
		goto done;
	}

	if(line_count == 127)
		line_flag = 0;
	else line_flag = 1;

	if(!shift && line_flag)
	{	

		/*	checking numbers	*/		
			if((to_print != 0x0E) && (to_print != 0x1C) && !(to_print & 0x80) && (to_print != 0x2A) && (to_print != 0x36) && (to_print != 0x3A))
			{
				int printchar = kbdus[to_print];
				if(printchar >= 97 && printchar <=122)
					printchar = printchar - offset;
				setb(buffer, printchar);
			}
	 		else if(to_print == 0x0E)
	 		{	
	 			backspace(buffer, line_count);
	 			if(limit >= 0) limit = limit-2; else limit = 77;
	 			if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 		}
	 		else if(to_print == 0x1C)
	 		{	
	 			new_line(buffer);	
				clear_buf_line(buffer);	
				limit = 0;
				line_count = 0;
			}
	 		else
	 		{
				if(limit >= 0) limit--;
				else limit = 78;
				if(line_count >= 0) line_count--; else limit = -1;
			}
	}
	else if(shift && line_flag)
	{
		
		
		if((to_print != 0x0E) && (to_print != 0x1C) && !(to_print & 0x80) && (to_print != 0x2A) && (to_print != 0x36) && (to_print != 0x3A))
		{
			int printchar = kbdus_shift[to_print];
			if(printchar >= 65 && printchar <=90)
				printchar = printchar + offset;
			setb(buffer, printchar);
		}
	 	else if(to_print == 0x0E)
	 	{	
	 		backspace(buffer, line_count);
	 		if(limit >= 0) limit = limit-2; else limit = 77;
	 		if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 	}
	 	else if(to_print == 0x1C)
	 	{	
	 		new_line(buffer);	
			clear_buf_line(buffer);	
			limit = 0;
			line_count = 0;
		}
	 	else
	 	{
			if(limit >= 0) limit--;
			else limit = 78;
			if(line_count >= 0) line_count--; else limit = -1;
		}
	}

	else		// FULLY FILLED COMMAND LINE BUFFER
	{
		switch(to_print)
		{
			case 0x0E:	{	
	 					backspace(buffer, line_count);
	 					if(limit >= 0) limit = limit-1;
	 					if(line_count >= 0) line_count = line_count - 1;
	 					break;	
	 					}
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

	if(line_flag)
	{
		limit++;
		line_count++;
	}

	if(limit == 80)
	{
		new_line(buffer);
		clear_buf_line(buffer);
		limit = 0;
	}

	done:
		return;
//	index++;
//	if(index == 81)
											//inform when finished handling interrupt
	//enable_irq(1);										//unmask so that new interrupts can be taken care of
}

node* pass_buff()
{
	return buffer;
}

int pass_count()
{
	return line_count;
}

void cout(char *input)
{
	int len = strlen(input);
	int i = 0;
	new_line(buffer);
	for(i = 0; i < len; i++)
	{
		setb(buffer, input[i]);
		if(input[i] == '\n')
			new_line(buffer);
	}
	new_line(buffer);
	limit = 0;
	line_count = 0;
}

/* Dummy Interrupt handler */
/********************************************************
void													*
dummy_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		for system calls			 	 	*
*	Return Value:	void								*
*	Function: Should execute test_interrupts handler	*
********************************************************/
/* Prints int # */
void dummy_int_handler()
{
	printf("DUMMY INTERRUPT HANDLER");
}
