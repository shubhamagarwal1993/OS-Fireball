#ifndef _MUSIC_H
#define _MUSIC_H

#include "types.h"

void play_sound(uint32_t sound_freq);
void nosound();
void beep();
extern int32_t music(int to_print);

#endif /* _MUSIC_H */
