#define ASM 	1

kernel_ds:
	.long 0x0018

#include "wrapper.h"
#include "keyboard.h"


.text

.globl kbd_wrapper, rtc_wrapper

kbd_wrapper:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	call kbd_int_handler
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
	; pushal
	; call kbd_int_handler
	; popal
	; iret



rtc_wrapper:
	pushal
	call rtc_int_handler
	popal
	iret
