#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

volatile int32_t interrupt_number;
int test_flag = 0;

#define INIT_RATE		0x0E
#define PORT_1			0x70
#define PORT_2			0x71
#define VAL_1			0x8A
#define VAL_2			0x8B
#define BIT_6			0x40
#define FIRST_4_BITS	0xF0
#define HIGHEST_FREQ	1024
#define LOWEST_FREQ		2
#define HIGH_16_BIT		32768
#define RTC_PORT_NO		8
#define REG_C			0x0C
#define RTC_READ_BYTES	4
#define RET_SUCCESS		0
#define RET_FAILURE		-1
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

	int rate = INIT_RATE;								// rate must be above 2 and not over 15
	outb(VAL_1, PORT_1);								// set index to register A, disable NMI
	char prev = inb(PORT_2);							// get initial value of register A
	outb(VAL_1, PORT_1);								// reset index to A
	outb((prev & FIRST_4_BITS) | rate, PORT_2); 		//write only our rate to A. Note, rate is the bottom 4 bits.

	outb(VAL_2, PORT_1);								// select register B, and disable NMI
	prev = inb(PORT_2);									// read the current value of register B
	outb(VAL_2, PORT_1);								// set the index again (a read will reset the index to register D)
	outb(prev | BIT_6, PORT_2);							// write the previous value ORed with 0x40. This turns on bit 6 of register B
	interrupt_number = 0;
	enable_irq(RTC_PORT_NO);							//the offset of the RTC is IRQ1
                                                                                                                                             
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
	//test_interrupts();								//calls function that writes video memory to screen.
	//test_rtc();
	outb(REG_C, PORT_1);								// select register C
	inb(PORT_2);										// just throw away contents
	interrupt_number++;
	send_eoi(RTC_PORT_NO);
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
	// file_t* file;
	// uint8_t* buf = "INTERRUPT";
	// int32_t count;
	// rtc_write(file, buf, 4);
	// while(interrupt_number < 20)
	// {
	// 	terminal_write(file, buf, 15);
	// 	rtc_read(file, buf, count);
	// }
	return 0;
}

void disable_rtc_test()
{
	test_flag = 0;
}

int32_t rtc_open(file_t* file, const uint8_t* filename)
{
	file->file_op = &rtc_jt;						//This initializes the file_op
	file->inode_ptr = NULL;							//This initializes the file_op
	file->file_pos = 0;								//This initializes the file position to start from the beginning
	file->flags = 1;								//This initializes flags
	init_rtc();										//This sets the rtc rate
	return RET_SUCCESS;								//return 0 if all fine
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
	sti();											//Read interrupts
	uint32_t temp = interrupt_number;				//This sets the initial interrupt number which is incremented in the rtc handler
	//temp = 0;
	while(temp == interrupt_number);				//when interrupt number changes(increases) break out of loop -> till then keep waiting for an rtc interrupt
	return RET_SUCCESS;								//return 0 if all fine
}


/************************************************
rtc_write - change the clock frequency
Probably make a function call in kernel to show that frequency can be changed
*************************************************/
int32_t rtc_write(file_t* file, const uint8_t* buf, int32_t nbytes)
{	
	if(nbytes != RTC_READ_BYTES)
		return RET_FAILURE;

	uint32_t flags;
	cli_and_save(flags);								//save all the flags and stop interrupts	

	uint32_t frequency = *(uint32_t*)buf;

	if((frequency > HIGHEST_FREQ)||(frequency < LOWEST_FREQ))			//if frequency is out of bounds then return -1
	{
		restore_flags(flags);							//restore the flags even if we are returning -1
		return RET_FAILURE;
	}
	
	//printf("inside rtc_write\n");
	int rate = LOWEST_FREQ;								// rate must be above 2 and not over 15
	while(frequency != HIGH_16_BIT >> (rate-1))			//rate calculation formula on OS DEV
	{
		rate++;
	}
 	rate--;

	outb(VAL_1, PORT_1);								// set index to register A, disable NMI
	char prev = inb(PORT_2);							// get initial value of register A
	outb(VAL_1, PORT_1);								// reset index to A
	outb((prev & FIRST_4_BITS) | rate, PORT_2); 		//write only our rate to A. Note, rate is the bottom 4 bits.

	outb(VAL_2, PORT_1);								// select register B, and disable NMI
	prev = inb(PORT_2);									// read the current value of register B
	outb(VAL_2, PORT_1);								// set the index again (a read will reset the index to register D)
	outb(prev | BIT_6, PORT_2);							// write the previous value ORed with 0x40. This turns on bit 6 of register B
														//the offset of the RTC is IRQ1
	sti();
	restore_flags(flags);
	return RET_SUCCESS;
}

int32_t rtc_close(file_t* file)
{	
// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
	return RET_SUCCESS;
}
