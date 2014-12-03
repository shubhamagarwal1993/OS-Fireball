#include "terminal.h"
#include "keyboard.h"
#include "filesys.h"
#include "lib.h"

#define VIDEO  0xB8000  
static char* video_mem = (char *)VIDEO;
#define ATTRIB_COLOUR_2  0xE4
#define ATTRIB_COLOUR  0x4E

void boot_screen(){

//	char* bit_map = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ||||||||                 ||             || ||   |||||| |||||                    |||||||| ||              ||             || ||   ||  || ||                       ||                       ||             || ||   ||  || |||||                    ||||||   || |||||| ||||| ||||||| |||||| || ||   ||  ||    ||                    ||       || ||   | || || ||   ||     || || ||   |||||| |||||                    ||       || ||     ||||| ||   || |||||| || ||                                   ||       || ||     ||    ||   || ||  || || ||                                   ||       || ||     ||||| ||||||| |||||| || ||                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ";
//	char* bit_map = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ||||||||                 ||             || ||   |||||| |||||                    |||||||| ||              ||             || ||   ||  || ||                       ||                       ||             || ||   ||  || |||||                    ||||||   || |||||| ||||| ||||||| |||||| || ||   ||  ||    ||                    ||       || ||   | || || ||   ||     || || ||   |||||| |||||                    ||       || ||     ||||| ||   || |||||| || ||                                   ||       || ||     ||    ||   || ||  || || ||                                   ||       || ||     ||||| ||||||| |||||| || ||                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ";
	//^better centred
	char* bit_map = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ||||||||                 ||             || ||   |||||| |||||                    |||||||| ||              ||             || ||   ||  || ||                       ||                       ||             || ||   ||  || |||||                    ||||||   || |||||| ||||| ||||||| |||||| || ||   ||  ||    ||                    ||       || ||  || || || ||   ||     || || ||   |||||| |||||                    ||       || ||     ||||| ||   || |||||| || ||                                   ||       || ||     ||    ||   || ||  || || ||                                   ||       || ||     ||||| ||||||| |||||| || ||                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ";	
	int i = 0;
	for(i = 0; i < strlen(bit_map); i++)
	{
	//	buf[i].mo = bit_map[i];
		//printf("%c", bit_map[i]);
		if(i%80 == 0)
		{
        	screen_y++;
        	screen_x = 0;
    	}
		//*(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_BLUE;
		if(bit_map[i] == '|')
    		*(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_COLOUR_2;
    	else
    		*(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_COLOUR;
	}
}


