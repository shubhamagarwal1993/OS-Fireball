#include "keyboard.h"
#include "i8259.h"
#include "buffer.h"
#include "terminal.h"

/*
	LOLWUT EDGE CASE
	When you are holding backspace and you go from 79 to 78 on the x position i.e - 79-78 on the buffer, when you start typing again, you type on the same line, and overwrite the
	old values. Basically new_line does not get called when you start typing after holding down backspace and you delete beyond 78

*/

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
	}
	reset_flag = 1;

	int to_print;										//disable line so we can complete this before handling some other interrupt 	
	to_print = inb(0x60);
	kbd_logic(to_print);
	//printb(buffer);
	//init_terminal();
	
	//test_filesys();
	//terminal_open(to_print);
	test_read_write(buffer, to_print);
	
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
	if(shift) offset = 32;

	//shift disabled
	if(to_print == 0xAA || to_print == 0xB6)
	{
		offset = 0;
		shift = 0;
	}

	if(to_print == 0x3A)
	{
		caps_count++;
		if(caps_count%2 == 1)
			offset = 32;
		else offset = 0;
	}	
	
	if(to_print == 0x1D)
	{
		ctrl = 1;
	}

	if(to_print == 0x9D)		
	{
		ctrl = 0;
	}

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
		switch(to_print)									//check which button was pressed on the keyboard
		{
		/*	checking numbers	*/		
			case 0x02:	{	setb(buffer, 49);	break;	}	//1
			case 0x03:	{	setb(buffer, 50);	break;	}	//2
			case 0x04:	{	setb(buffer, 51);	break;	}	//3		
			case 0x05:	{	setb(buffer, 52);	break;	}	//4
			case 0x06:	{	setb(buffer, 53);	break;	}	//5
			case 0x07:	{	setb(buffer, 54);	break;	}	//6
			case 0x08:	{	setb(buffer, 55);	break;	}	//7
			case 0x09:	{	setb(buffer, 56);	break;	}	//8
			case 0x0A:	{	setb(buffer, 57);	break;	}	//9
			case 0x0B:	{	setb(buffer, 48);	break;	}	//0
		
		/*	checking letters	*/	
			case 0x1E:	{	setb(buffer, 97 - offset);		break;	}						
			case 0x30:	{	setb(buffer,  98 - offset);		break;	}
			case 0x2E:	{	setb(buffer,  99 - offset);		break;	}
			case 0x20:	{	setb(buffer,  100 - offset);	break;	}
			case 0x12:	{	setb(buffer,  101 - offset);	break;	}
			case 0x21:	{	setb(buffer,  102 - offset);	break;	}
			case 0x22:	{	setb(buffer,  103 - offset);	break;	}
			case 0x23:	{	setb(buffer,  104 - offset);	break;	}
			case 0x17:	{	setb(buffer,  105 - offset);	break;	}				
			case 0x24:	{	setb(buffer,  106 - offset);	break;	}
			case 0x25:	{	setb(buffer,  107 - offset);	break;	}
			case 0x26:	{	setb(buffer,  108 - offset);	break;	}
			case 0x32:	{	setb(buffer,  109 - offset);	break;	}
			case 0x31:	{	setb(buffer,  110 - offset);	break;	}
			case 0x18:	{	setb(buffer,  111 - offset);	break;	}
			case 0x19:	{	setb(buffer,  112 - offset);	break;	}
			case 0x10:	{	setb(buffer,  113 - offset);	break;	}
			case 0x13:	{	setb(buffer,  114 - offset);	break;	}		
			case 0x1F:	{	setb(buffer,  115 - offset);	break;	}				
			case 0x14:	{	setb(buffer,  116 - offset);	break;	}
			case 0x16:	{	setb(buffer,  117 - offset);	break;	}
			case 0x2F:	{	setb(buffer,  118 - offset);	break;	}
			case 0x11:	{	setb(buffer,  119 - offset);	break;	}
			case 0x2D:	{	setb(buffer,  120 - offset);	break;	}
			case 0x15:	{	setb(buffer,  121 - offset);	break;	}
			case 0x2C:	{	setb(buffer,  122 - offset);	break;	}		
	/*	checking special characters	*/	
			case 0x0C:	{	setb(buffer, 45);	break;	}	//"-"
	 		case 0x0D:	{	setb(buffer, 61);	break;	}	//"="
	 		case 0x1A:	{	setb(buffer, 91);	break;	}	//"["
	 		case 0x1B:	{	setb(buffer, 93);	break;	}	//"]"
	 		case 0x27:	{	setb(buffer, 59);	break;	}	//";"
	 		case 0x28:	{	setb(buffer, 39);	break;	}	//"'"
	 		case 0x29:	{	setb(buffer, 96);	break;	}	//"`"
	 		case 0x2B:	{	setb(buffer, 92);	break;	}	
	 		case 0x33:	{	setb(buffer, 44);	break;	}	//","
	 		case 0x34:	{	setb(buffer, 46);	break;	}	//"."
			case 0x35:	{	setb(buffer, 47);	break;	}	//"/"
			case 0x39:	{	setb(buffer, 32);	break;	}	//" "
	 		case 0x0E:	{	
	 						backspace(buffer, line_count);
	 						if(limit >= 0) limit = limit-2; else limit = 77;
	 						if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 						break;	
	 					}
	 		case 0x1C:	{	
	 						new_line(buffer);	
							clear_buf_line(buffer);	
							limit = -1;
							line_count = -1;
							break;	
						}
	 		default:	{
							if(limit >= 0) limit--;
							else limit = 78;
							if(line_count >= 0) line_count--; else limit = -1;
						}
		}
	}
	else if(shift && line_flag)
	{
		switch(to_print)									//check which button was pressed on the keyboard
		{
				/*	checking numbers	*/		
		case 0x02:	{	setb(buffer, 33);	break;	}	//"!"
		case 0x03:	{	setb(buffer, 64);	break;	}	//"@"
		case 0x04:	{	setb(buffer, 35);	break;	}	//"#"		
		case 0x05:	{	setb(buffer, 36);	break;	}	//"$"
		case 0x06:	{	setb(buffer, 37);	break;	}	//"%"
		case 0x07:	{	setb(buffer, 94);	break;	}	//"^"
		case 0x08:	{	setb(buffer, 38);	break;	}	//"&"
		case 0x09:	{	setb(buffer, 42);	break;	}	//"*"
		case 0x0A:	{	setb(buffer, 40);	break;	}	//"("
		case 0x0B:	{	setb(buffer, 41);	break;	}	//")"
	/*	checking letters	*/	

		case 0x1E:	{	setb(buffer,  97 - offset);		break;	}						
		case 0x30:	{	setb(buffer,  98 - offset);		break;	}
		case 0x2E:	{	setb(buffer,  99 - offset);		break;	}
		case 0x20:	{	setb(buffer,  100 - offset);	break;	}
		case 0x12:	{	setb(buffer,  101 - offset);	break;	}
		case 0x21:	{	setb(buffer,  102 - offset);	break;	}
		case 0x22:	{	setb(buffer,  103 - offset);	break;	}
		case 0x23:	{	setb(buffer,  104 - offset);	break;	}
		case 0x17:	{	setb(buffer,  105 - offset);	break;	}				
		case 0x24:	{	setb(buffer,  106 - offset);	break;	}
		case 0x25:	{	setb(buffer,  107 - offset);	break;	}
		case 0x26:	{	setb(buffer,  108 - offset);	break;	}
		case 0x32:	{	setb(buffer,  109 - offset);	break;	}
		case 0x31:	{	setb(buffer,  110 - offset);	break;	}
		case 0x18:	{	setb(buffer,  111 - offset);	break;	}
		case 0x19:	{	setb(buffer,  112 - offset);	break;	}
		case 0x10:	{	setb(buffer,  113 - offset);	break;	}
		case 0x13:	{	setb(buffer,  114 - offset);	break;	}		
		case 0x1F:	{	setb(buffer,  115 - offset);	break;	}				
		case 0x14:	{	setb(buffer,  116 - offset);	break;	}
		case 0x16:	{	setb(buffer,  117 - offset);	break;	}
		case 0x2F:	{	setb(buffer,  118 - offset);	break;	}
		case 0x11:	{	setb(buffer,  119 - offset);	break;	}
		case 0x2D:	{	setb(buffer,  120 - offset);	break;	}
		case 0x15:	{	setb(buffer,  121 - offset);	break;	}
		case 0x2C:	{	setb(buffer,  122 - offset);	break;	}	

/*	checking special characters	*/	
 		case 0x0C:	{	setb(buffer, 95);	break;	}	//"_"
 		case 0x0D:	{	setb(buffer, 43);	break;	}	//"+"
 		case 0x1A:	{	setb(buffer, 123);	break;	}	//"{"
 		case 0x1B:	{	setb(buffer, 125);	break;	}	//"}"
 		case 0x27:	{	setb(buffer, 58);	break;	}	//":"
 		case 0x28:	{	setb(buffer, 34);	break;	}
 		case 0x29:	{	setb(buffer, 126);	break;	}	//"~"
 		case 0x2B:	{	setb(buffer, 124);	break;	}	//"|"
 		case 0x33:	{	setb(buffer, 60);	break;	}	//"<"
 		case 0x34:	{	setb(buffer, 62);	break;	}	//">"
		case 0x35:	{	setb(buffer, 63);	break;	}	//"?"
		case 0x39:	{	setb(buffer, 32);	break;	}	//" "
		case 0x0E:	{	
	 					backspace(buffer, line_count);
	 					if(limit >= 0) limit = limit-2; else limit = 77;
	 					if(line_count >= 0) line_count = line_count - 2; else limit = -1;
	 					break;	
	 				}
		case 0x1C:	{	
						new_line(buffer);
						clear_buf_line(buffer);	
						limit = -1;
						line_count = -1;
						break;	}
		default:	{
						if(limit >= 0) limit--;
						else limit = 78;
						if(line_count >= 0) line_count--; else limit = -1;
					}
				
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
