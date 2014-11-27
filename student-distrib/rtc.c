#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

volatile int32_t interrupt_number = 0;
int test_flag = 0;

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
	// disable_irq(8);
	cli();
//	printf("RTC INTERRUPT HANDLER");
	//test_interrupts();									//calls function that writes video memory to screen.
	interrupt_number++;
	//test_rtc();
	outb(0x0C, 0x70);									// select register C
	inb(0x71);											// just throw away contents
	// enable_irq(8);
	sti();
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
void test_rtc()
{
	if(interrupt_number > 20)
		rtc_write(NULL, 64);
	if(test_flag!=0)
		printf("%d", interrupt_number);
}

void disable_rtc_test()
{
	test_flag = 0;
}

int32_t rtc_open()
{//	printf("inside rtc_open\n");
	return 0;
}

int32_t rtc_read(char* buf, int count)
{	//printf("inside rtc_read\n");
	//For RTC, this call should always return 0, but only after an interrupt has occurred.
	//set a flag and wait until the interrupt handler clears it then return 0
	enable_irq(8);
	while(interrupt_number == 0)
	{
		if(interrupt_number > 0)
			break;
	} 
	interrupt_number = 0;
	return 0;
}

/************************************************
demonstrate that you can change the clock frequency
Probably make a function call in kernel to show that frequency can be changed
*************************************************/
int32_t rtc_write(char* buf, int32_t frequency)
{	//printf("inside rtc_write\n");
//In the case of the RTC, the system call should always accept only a 4-byte
// 		integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly.
	int rate = 2;							// rate must be above 2 and not over 15
	while(frequency != 32768 >> (rate-1))
	{
		rate++;
	}
//	printf(" Rate is %d", rate);
 
	// disable_irq(8);
	outb(0x8A, 0x70);								// set index to register A, disable NMI
	char prev = inb(0x71);							// get initial value of register A
	outb(0x8A, 0x70);								// reset index to A
	outb((prev & 0xF0) | rate, 0x71); 				//write only our rate to A. Note, rate is the bottom 4 bits.

	outb(0x8B, 0x70);								// select register B, and disable NMI
	prev = inb(0x71);								// read the current value of register B
	outb(0x8B, 0x70);								// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, 0x71);						// write the previous value ORed with 0x40. This turns on bit 6 of register B
	// enable_irq(8);									//the offset of the RTC is IRQ1
//	printf("finished rtc_write\n");
	return 0;
}

int32_t rtc_close()
{	//printf("inside rtc_close\n");
// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
	return 0;
}
