#ifndef _WRAPPER_H
#define _WRAPPER_H

#ifndef ASM


void kbd_wrapper();

void rtc_wrapper();

void syscall_handler();

void jump_to_userspace();

#endif /* ASM */

#endif /* _WRAPPER_H */
