#include "pit.h"
#include "wrapper.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "terminal.h"
#include "systemcalls.h"

#define PIT_CHANNEL_ZERO		0
#define PIT_CHANNEL_TWO			2
#define SHIFT_BY_8				8
#define CHANNEL_ZERO_DATA_PORT	0x40
#define CHANNEL_ONE_DATA_PORT	0x41
#define CHANNEL_TWO_DATA_PORT	0x42
#define MODE_COMMAND_REGISTER	0x43
#define HIGHEST_FREQ			1193180	
#define ALL_HIGH_BITS			0xFF
#define PIT_COMMAND_REGISTER	0x36
#define RET_SUCCESS				0
#define RET_FAILURE				-1

int32_t pit_int_num;
//status = " terminal1  terminal2  terminal3                                                ";

void init_pit(int32_t channel, int32_t freq)
{
	enable_irq(PIT_CHANNEL_ZERO);
	uint16_t data = HIGHEST_FREQ/ freq;
	if(channel == PIT_CHANNEL_ZERO)
	{
		// uint8_t low_byte = 11930 & 0xFF;
		// uint8_t high_byte = (11930 >> 8) & 0xFF;
		outb(PIT_COMMAND_REGISTER, MODE_COMMAND_REGISTER);								// PIT Command Port
		outb((data & ALL_HIGH_BITS), CHANNEL_ZERO_DATA_PORT);							// PIT data port Channel 0
		outb(((data >> SHIFT_BY_8) & ALL_HIGH_BITS), CHANNEL_ZERO_DATA_PORT);			// PIT data port Channel 0
	}
	else if(channel == PIT_CHANNEL_TWO)
	{
		outb(PIT_COMMAND_REGISTER, MODE_COMMAND_REGISTER);								// PIT Command Port
		outb((data & ALL_HIGH_BITS), CHANNEL_TWO_DATA_PORT);							// PIT data port Channel 2
		outb(((data >> SHIFT_BY_8) & ALL_HIGH_BITS), CHANNEL_TWO_DATA_PORT);			// PIT data port Channel 2
	}
	else 
		return;
}

void pit_int_handler()
{
	cli();
	pit_int_num++;										//increase PIT number whenever we get a PIT interrupt->this will help with clock frequency
	//scheduling();									
	status_bar();										//display the status bar with the time displayed
	if(pit_int_num%(100) == 0)							//This decides when the time on the clock will increase by 1 sec
	{	
		status = embed_time(status);								//put this time on the status bar
	}	
	sti();													
	send_eoi(PIT_CHANNEL_ZERO);										
	return;
}
