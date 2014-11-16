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
    movl 4(%esp), %ebx
    movl $0x23, %ax
    movl %ax, %ds 	
    movl %ax, %es 	
    movl %ax, %fs 	
    movl %ax, %gs 	
                   		
    movl %esp, %eax
    pushl $0x23	
    pushl %eax 		
    pushf
    pushl $0x1B
    pushl %ebx 		
    iret		
   					 

syscall_handler:
	cmpl $1, %eax
	jl call_fail
	addl $-1, %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	call *jump_table(, %eax, 4)
	jmp end_call

call_fail:
	movl $-1, %eax

end_call:
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	iret

jump_table:
.long 0x0, execute
