#include "pit.h"
#include "wrapper.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "terminal.h"
#include "systemcalls.h"

int32_t pit_int_num;

void init_pit(int32_t channel, int32_t freq)
{
	enable_irq(0);
	uint16_t data = 1193180/ freq;
	if(channel == 0)
	{
		// uint8_t low_byte = 11930 & 0xFF;
		// uint8_t high_byte = (11930 >> 8) & 0xFF;
		outb(0x36, 0x43);	// PIT Command Port
		outb((data & 0xFF), 0x40);	// PIT data port Channel 0
		outb(((data >> 8) & 0xFF), 0x40);	// PIT data port Channel 0
	}
	else if(channel == 2)
	{
		outb(0x36, 0x43);	// PIT Command Port
		outb((data & 0xFF), 0x42);	// PIT data port Channel 2
		outb(((data >> 8) & 0xFF), 0x42);	// PIT data port Channel 2
	}
	else 
		return;
}

void pit_int_handler()
{
	cli();
	pit_int_num++;
	status_bar();
	//scheduling();
	if(pit_int_num%(6000) == 0)
	{	
		embed_time(status);
	}	
	sti();
	send_eoi(0);
	return;
}
