#define ASM 	1
kernel_ds:
	.long 0x0018
#include "wrapper.h"
.text
.globl kbd_wrapper, rtc_wrapper, jump_to_userspace, syscall_handler, jump_table
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
jump_to_userspace:
 	
 	cli
    movl 32(%esp), %ebx
    mov $0x2B, %ax
    mov %ax, %ds	
                   		
    movl %esp, %eax
    
	pushl $0x2B
    pushl $0x083FFFFC		
    pushf
    popl %eax
    orl $0xC2, %eax
    pushl %eax
    pushl $0x23
    pushl %ebx 	
    sti	
    iret		
   					 
syscall_handler:
	cmpl $1, %eax
	jl call_fail
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushl %edx
	pushl %ecx
	pushl %ebx
	call *jump_table(, %eax, 4)
	jmp end_call
call_fail:
	movl $-1, %eax
end_call:
	popl %ebx
	popl %ecx
	popl %edx
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	iret
jump_table:
.long 0x0, halt, execute, read, write, open, close, getargs, vidmap
