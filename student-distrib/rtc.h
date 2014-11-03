#ifndef _rtc_H
#define _rtc_H
//#ifndef ASM	

#include "wrapper.h"
#include "types.h"

extern void rtc_int_handler();

int32_t rtc_open();
int32_t rtc_read();
int32_t rtc_write(uint32_t frequency);
int32_t ece391_close();

int32_t rtc_open();
int32_t rtc_read();
int32_t rtc_write(uint32_t frequency);
int32_t rtc_close();

extern void test_rtc();
extern void disable_rtc_test();

//#endif	/* ASM */

#endif /* _rtc_H */
