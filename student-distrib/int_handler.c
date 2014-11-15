#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "int_handler.h"
#include "wrapper.h"
#include "keyboard.h"

void set_idt()
{
	set_exception();
	set_interrupt();
}

/* Exception handling functions below */
void set_exception()
{
	idt_desc_t idt_entry;
	idt_entry.seg_selector = KERNEL_CS;
	idt_entry.dpl = 0;
	idt_entry.present = 1;
	idt_entry.size = 1;
	idt_entry.reserved0	= 0;
	idt_entry.reserved1 = 1;
	idt_entry.reserved2 = 1;
	idt_entry.reserved3 = 1;
	
	int i = 0;
	for(i = 0; i < 20; i++)
	{
		if(i != 1 && i != 15)
			idt[i] = idt_entry;
	}

	SET_IDT_ENTRY(idt[0], divide_error);
	SET_IDT_ENTRY(idt[2], NMI_Interrupt);
	SET_IDT_ENTRY(idt[3], breakpoint);
	SET_IDT_ENTRY(idt[4], overflow);
	SET_IDT_ENTRY(idt[5], BOUND_range_exceeded);
	SET_IDT_ENTRY(idt[6], invalid_opcode);
	SET_IDT_ENTRY(idt[7], device_not_available);
	SET_IDT_ENTRY(idt[8], double_fault);
	SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun);
	SET_IDT_ENTRY(idt[10], invalid_TSS);
	SET_IDT_ENTRY(idt[11], segment_not_present);
	SET_IDT_ENTRY(idt[12], stack_segment_fault);
	SET_IDT_ENTRY(idt[13], general_protection);
	SET_IDT_ENTRY(idt[14], page_fault);
	SET_IDT_ENTRY(idt[16], floating_point_error);
	SET_IDT_ENTRY(idt[17], alignment_check);
	SET_IDT_ENTRY(idt[18], machine_check);
	SET_IDT_ENTRY(idt[19], floating_point_exception);
}

/* Interrupt handling functions below */
void set_interrupt()
{
		idt_desc_t idt_entry;
		idt_entry.seg_selector = KERNEL_CS;
		idt_entry.dpl = 0;
		idt_entry.present = 1;
		idt_entry.size = 1;
		idt_entry.reserved0	= 0;
		idt_entry.reserved1 = 1;
		idt_entry.reserved2 = 1;
		idt_entry.reserved3 = 0;
		idt[33] = idt_entry; 
		idt[40] = idt_entry; 
		SET_IDT_ENTRY(idt[40], &rtc_wrapper);
		SET_IDT_ENTRY(idt[33], &kbd_wrapper);
		SET_IDT_ENTRY(idt[128], &syscall_handler);		
}


/* Exception handling functions below */

void divide_error()
{
	disable_irq(1);
	clear_blue();
	printf("Divide error!");
	while(1){}
}

void NMI_Interrupt()
{
	disable_irq(1);
	clear_blue();	
	printf("NMI Interrupt!");
	while(1){}
}

void breakpoint()
{
	disable_irq(1);
	clear_blue();
	printf("breakpoint!");
	while(1){}
}

void overflow()
{
	disable_irq(1);
	clear_blue();
	printf("overflow!");
	while(1){}
}

void BOUND_range_exceeded()
{
	disable_irq(1);
	clear_blue();
	printf("BOUND range exceeded!");
	while(1){}
}

void invalid_opcode()
{
	disable_irq(1);
	clear_blue();
	printf("Invalid Opcode!");
	while(1){}
}

void device_not_available()
{
	disable_irq(1);
	clear_blue();
	printf("Device Not available");
	while(1){}
}

void double_fault()
{
	disable_irq(1);
	clear_blue();
	printf("Double Fault!");
	while(1){}
}

void coprocessor_segment_overrun()
{
	disable_irq(1);
	clear_blue();
	printf("coprocessor_segment_overrun!");
	while(1){}
}

void invalid_TSS()
{
	disable_irq(1);
	clear_blue();
	printf("Invalid TSS!");
	while(1){}
}

void segment_not_present()
{
	disable_irq(1);
	clear_blue();
	printf("Segment not present!");
	while(1){}
}

void stack_segment_fault()
{
	disable_irq(1);
	clear_blue();
	printf("Stack Segment Fault!");
	while(1){}
}

void general_protection()
{
	disable_irq(1);
	clear_blue();
	printf("General Protection");
	while(1){}
}

void page_fault()
{
	disable_irq(1);
//	clear_blue();
	printf("Page Fault!");
	while(1){}
}

void floating_point_error()
{
	disable_irq(1);
	clear_blue();
	printf("Floating Point Error!");
	while(1){}
}

void alignment_check()
{
	disable_irq(1);
	clear_blue();
	printf("Alignment Check!");
	while(1){}
}

void machine_check()
{
	disable_irq(1);
	clear_blue();
	printf("Machine Check");
	while(1){}
}

void floating_point_exception()
{
	disable_irq(1);
	clear_blue();
	printf("Floating Point Exception");
	while(1){}
}

