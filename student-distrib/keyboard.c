#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

int caps_count = 0;
int offset = 0;

/* Keyboard Interrupt handler */
/********************************************************
void													*
kbd_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		prints pressed button on screen 	*
*	Return Value:	void								*
*	Function: Prints out character typed to screen		*
********************************************************/
void kbd_int_handler()
{
	disable_irq(1);										//IRQ line was 1 so we get the port numbers
	int to_print;										//disable line so we can complete this before handling some other interrupt 	
	to_print = inb(0x60);								//store received info into char to check with which key was pressed 
	
	if(to_print == 0x3A)
	{
		caps_count++;
		if(caps_count%2 == 1)
			offset = 32;
		else offset = 0;
	}	
		
	switch(to_print)									//check which button was pressed on the keyboard
	{
	/*	checking numbers	*/		
		case 0x02:	{	printf("1");	break;	}
		case 0x03:	{	printf("2");	break;	}
		case 0x04:	{	printf("3");	break;	}			
		case 0x05:	{	printf("4");	break;	}
		case 0x06:	{	printf("5");	break;	}
		case 0x07:	{	printf("6");	break;	}
		case 0x08:	{	printf("7");	break;	}
		case 0x09:	{	printf("8");	break;	}	
		case 0x0A:	{	printf("9");	break;	}
		case 0x0B:	{	printf("0");	break;	}
	
	/*	checking letters	*/	
		case 0x1E:	{	printf("%c", 97 - offset);	break;	}						
		case 0x30:	{	printf("%c", 98 - offset);	break;	}
		case 0x2E:	{	printf("%c", 99 - offset);	break;	}
		case 0x20:	{	printf("%c", 100 - offset);	break;	}
		case 0x12:	{	printf("%c", 101 - offset);	break;	}
		case 0x21:	{	printf("%c", 102 - offset);	break;	}
		case 0x22:	{	printf("%c", 103 - offset);	break;	}
		case 0x23:	{	printf("%c", 104 - offset);	break;	}
		case 0x17:	{	printf("%c", 105 - offset);	break;	}				
		case 0x24:	{	printf("%c", 106 - offset);	break;	}
		case 0x25:	{	printf("%c", 107 - offset);	break;	}
		case 0x26:	{	printf("%c", 108 - offset);	break;	}
		case 0x32:	{	printf("%c", 109 - offset);	break;	}
		case 0x31:	{	printf("%c", 110 - offset);	break;	}
		case 0x18:	{	printf("%c", 111 - offset);	break;	}
		case 0x19:	{	printf("%c", 112 - offset);	break;	}
		case 0x10:	{	printf("%c", 113 - offset);	break;	}
		case 0x13:	{	printf("%c", 114 - offset);	break;	}		
		case 0x1F:	{	printf("%c", 115 - offset);	break;	}				
		case 0x14:	{	printf("%c", 116 - offset);	break;	}
		case 0x16:	{	printf("%c", 117 - offset);	break;	}
		case 0x2F:	{	printf("%c", 118 - offset);	break;	}
		case 0x11:	{	printf("%c", 119 - offset);	break;	}
		case 0x2D:	{	printf("%c", 120 - offset);	break;	}
		case 0x15:	{	printf("%c", 121 - offset);	break;	}
		case 0x2C:	{	printf("%c", 122 - offset);	break;	}		
/*	checking special characters	*/	
		case 0x0C:	{	//printf("-");	
						int * ptr = NULL;
						printf("%d", *ptr);
						break;	}
 		case 0x0D:	{	printf("=");	break;	}	
 		case 0x1A:	{	printf("[");	break;	}
 		case 0x1B:	{	printf("]");	break;	}
 		case 0x27:	{	printf(";");	break;	}
 		case 0x28:	{	printf("'");	break;	}
 		case 0x29:	{	printf("`");	break;	}
 		case 0x33:	{	printf(",");	break;	}
 		case 0x34:	{	printf(".");	break;	}
		case 0x35:	{	printf("/");	break;	}
		case 0x39:	{	printf(" ");	break;	}
 		case 0x0E:	{	/*printf("");*/
						// int ptr1 = 45;
						// int a = ptr1 / 0;
						// printf("%d",a);
 						
						 int * ptr = 0;
						 int val = *ptr;
						 printf("%d", ptr);	
 						
						//int a, b, c; // some integers
						//int *pi;     // a pointer to an integer

						//a = 5;
						//pi = &a; // pi points to a
						//b = *pi; // b is now 5
						//pi = NULL;
						//c = *pi; // this is a NULL pointer dereference
						//printf("%d", c);
 						break;	}
	}
//	index++;
//	if(index == 81)
	send_eoi(1);										//inform when finished handling interrupt
	enable_irq(1);										//unmask so that new interrupts can be taken care of
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
