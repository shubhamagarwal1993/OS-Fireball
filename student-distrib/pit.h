#ifndef _PIT_H
#define _PIT_H

#include "types.h"

void init_pit(int32_t channel, int32_t freq);
void pit_int_handler();

#endif /* _PIT_H */
