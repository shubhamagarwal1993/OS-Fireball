#ifndef _rtc_H
#define _rtc_H
//#ifndef ASM	

#include "wrapper.h"
#include "types.h"

extern void rtc_int_handler();


int32_t rtc_open();
int32_t rtc_read();
int32_t rtc_write(uint32_t frequency);
int32_t rtc_close();

int32_t ece391_open(const uint8_t* filename);
int32_t ece391_read(int32_t fd, void* buf, int32_t nbytes);
int32_t ece391_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t ece391_close(int32_t fd);

extern void test_rtc();

//#endif	/* ASM */

#endif /* _rtc_H */
