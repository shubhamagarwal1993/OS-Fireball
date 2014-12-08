#ifndef _WRAPPER_H
#define _WRAPPER_H

#ifndef ASM

#include "types.h"

void kbd_wrapper();

void rtc_wrapper();

void pit_wrapper();

void syscall_handler();

void jump_to_userspace(uint32_t entry_addr);

void switch_linkage(uint32_t ss0, uint32_t esp0);

#endif /* ASM */

#endif /* _WRAPPER_H */
