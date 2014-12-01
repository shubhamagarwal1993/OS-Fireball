#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

volatile int32_t interrupt_number;
int test_flag = 0;

/****************************************
void									*
init_rtc(void)							*	
*	Inputs:			void				*
*	Outputs:		initialize RTC 		*
*	Return Value:	void				*
*	Function: Initialize the RTC 	 	*
****************************************/ 
void
init_rtc(void)		
{

	int rate = 0x0E;								// rate must be above 2 and not over 15
	outb(0x8A, 0x70);								// set index to register A, disable NMI
	char prev = inb(0x71);							// get initial value of register A
	outb(0x8A, 0x70);								// reset index to A
	outb((prev & 0xF0) | rate, 0x71); 				//write only our rate to A. Note, rate is the bottom 4 bits.

	outb(0x8B, 0x70);								// select register B, and disable NMI
	prev = inb(0x71);								// read the current value of register B
	outb(0x8B, 0x70);								// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, 0x71);						// write the previous value ORed with 0x40. This turns on bit 6 of register B
	interrupt_number = 0;
	enable_irq(8);									//the offset of the RTC is IRQ1
                                                                                                                                             
	return;
}



//int interrupt_number = 0;
/* RTC Interrupt handler */
/********************************************************
void													*
rtc_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		writes video memory to screen 	 	*
*	Return Value:	void								*
*	Function: Should execute test_interrupts handler	*
********************************************************/
void rtc_int_handler()
{
	//test_interrupts();									//calls function that writes video memory to screen.
	//test_rtc();
	outb(0x0C, 0x70);									// select register C
	inb(0x71);											// just throw away contents
	interrupt_number++;
	send_eoi(8);
}

/*	This is just for testing the RTC & shows up initially before typing	*/
/********************************************************
void													*
test_rtc()												*	
*	Inputs:			void								*
*	Outputs:		writes rtc output to screen  	 	*
*	Return Value:	void								*
*	Function: Should execute test_interrupts handler	*
********************************************************/
int32_t test_rtc_read()
{
	file_t* file;
	uint8_t* buf = "INTERRUPT";
	int32_t count;
	rtc_write(file, buf, 4);
	while(interrupt_number < 20)
	{
		terminal_write(file, buf, 15);
		rtc_read(file, buf, count);
	}
	return 0;
}

void disable_rtc_test()
{
	test_flag = 0;
}

int32_t rtc_open(file_t* file, const uint8_t* filename)
{
	file->file_op = &rtc_jt;
	file->inode_ptr = NULL;
	file->file_pos = 0;
	file->flags = 1;
	init_rtc();

	return 0;
}

int32_t rtc_read(file_t* file, uint8_t* buf, int32_t count)
{
	//printf("inside rtc_read\n");
	//enable_irq(8);
	//while(interrupt_number == 0)
	//{
	//	if(interrupt_number > 0)
	//		break;
	//} 
	//interrupt_number = 0;
	//interrupt_number++;
	sti();
	uint32_t temp = interrupt_number;
	//temp = 0;
	while(temp == interrupt_number)
	{
		if(inb(0x60) == 96) cout("tilde");
		break;
	}
	return 0;
}


/************************************************
demonstrate that you can change the clock frequency
Probably make a function call in kernel to show that frequency can be changed
*************************************************/
int32_t rtc_write(file_t* file, const uint8_t* buf, int32_t frequency)
{	
	uint32_t flags;
	cli_and_save(flags);
	//printf("inside rtc_write\n");
	int rate = 2;							// rate must be above 2 and not over 15
	while(frequency != 32768 >> (rate-1))
	{
		rate++;
	}
 	rate--;

	outb(0x8A, 0x70);								// set index to register A, disable NMI
	char prev = inb(0x71);							// get initial value of register A
	outb(0x8A, 0x70);								// reset index to A
	outb((prev & 0xF0) | rate, 0x71); 				//write only our rate to A. Note, rate is the bottom 4 bits.

	outb(0x8B, 0x70);								// select register B, and disable NMI
	prev = inb(0x71);								// read the current value of register B
	outb(0x8B, 0x70);								// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, 0x71);						// write the previous value ORed with 0x40. This turns on bit 6 of register B
								//the offset of the RTC is IRQ1
	sti();
	restore_flags(flags);
	return 0;
}

int32_t rtc_close(file_t* file)
{	
// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
	return 0;
}
