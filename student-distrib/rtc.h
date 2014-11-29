#ifndef _rtc_H
#define _rtc_H
//#ifndef ASM	

#include "wrapper.h"
#include "types.h"
#include "systemcalls.h"

extern void rtc_int_handler();

int32_t rtc_open(file_t* file, const uint8_t* filename);
int32_t rtc_read(file_t* file, uint8_t* buf, int32_t count);
int32_t rtc_write(file_t* file, const uint8_t* buf, int32_t frequency);
int32_t rtc_close(file_t* file);

extern driver_jt_t rtc_jt;
extern volatile int32_t interrupt_number;

extern void test_rtc();
extern void disable_rtc_test();

//#endif	/* ASM */

#endif /* _rtc_H */
