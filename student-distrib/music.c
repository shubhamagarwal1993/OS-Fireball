#include "keyboard.h"
#include "i8259.h"
#include "buffer.h"
#include "terminal.h"
#include "pit.h"
#include "music.h"


#define	C0 16.35
#define	Db0	17.32
#define	D0	18.35
#define	Eb0	19.45
#define	E0	20.60
#define	F0	21.83
#define	Gb0	23.12
#define	G0	24.50
#define	Ab0	25.96
#define	LA0	27.50
#define	Bb0	29.14
#define	B0	30.87
#define	C1	32.70
#define	Db1	34.65
#define	D1	36.71
#define	Eb1	38.89
#define	E1	41.20
#define	F1	43.65
#define	Gb1	46.25
#define	G1	49.00
#define	Ab1	51.91
#define	LA1	55.00
#define	Bb1	58.27
#define	B1	61.74
#define	C2	65.41
#define	Db2	69.30
#define	D2	73.42
#define	Eb2	77.78
#define	E2	82.41
#define	F2	87.31
#define	Gb2	92.50
#define	G2	98.00
#define	Ab2	103.83
#define	LA2	110.00
#define	Bb2	116.54
#define	B2	123.47
#define	C3	130.81
#define	Db3	138.59
#define	D3	146.83
#define	Eb3	155.56
#define	E3	164.81
#define	F3	174.61
#define	Gb3	185.00
#define	G3	196.00
#define	Ab3	207.65
#define	LA3	220.00
#define	Bb3	233.08
#define	B3	246.94
#define	C4	261.63
#define	Db4	277.18
#define	D4	293.66
#define	Eb4	311.13
#define	E4	329.63
#define	F4	349.23
#define	Gb4	369.99
#define	G4	392.00
#define	Ab4	415.30
#define	LA4	440.00
#define	Bb4	466.16
#define	B4	493.88
#define	C5	523.25
#define	Db5	554.37
#define	D5	587.33
#define	Eb5	622.25
#define	E5	659.26
#define	F5	698.46
#define	Gb5	739.99
#define	G5	783.99
#define	Ab5	830.61
#define	LA5	880.00
#define	Bb5	932.33
#define	B5	987.77
#define	C6	1046.50
#define	Db6	1108.73
#define	D6	1174.66
#define	Eb6	1244.51
#define	E6	1318.51
#define	F6	1396.91
#define	Gb6	1479.98
#define	G6	1567.98
#define	Ab6	1661.22
#define	LA6	1760.00
#define	Bb6	1864.66
#define	B6	1975.53
#define	C7	2093.00
#define	Db7	2217.46
#define	D7	2349.32
#define	Eb7	2489.02
#define	E7	2637.02
#define	F7	2793.83
#define	Gb7	2959.96
#define	G7	3135.96
#define	Ab7	3322.44
#define	LA7	3520.01
#define	Bb7	3729.31
#define	B7	3951.07
#define	C8	4186.01
#define	Db8	4434.92
#define	D8	4698.64
#define	Eb8	4978.03

//Play sound using built in speaker
void play_sound(uint32_t sound_freq) 
{
	cli();
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
    if(sound_freq != 0)
	 	Div = 1193180 / sound_freq;
	else
		Div = 1193180 / 30000; 

 	outb(0xB6, 0x43);
 	outb((uint8_t)(Div), 0x42);
 	outb((uint8_t)(Div >> 8), 0x42);
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) 
  		outb((tmp | 3), 0x61);
  	sti();
}
 

//stop sound 
void nosound() 
{
 	uint8_t tmp = (inb(0x61) & 0xFC);
  	outb(tmp, 0x61);
}

//play from the keypad
int32_t music(int to_print)
{
	int i = 0;

	if(to_print == 62)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 63)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 64)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 65)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 66)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 67)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 68)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 87)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	else if(to_print == 88)
	{
		for(i = 0; i < 10000; i++)
			play_sound(1000);
		nosound();
		return 1;
	}
	//nosound();
	else return 3;
}


//IMPERIAL SHIT
// the loop routine runs over and over again forever:
void imperial()
{
	int i;	
	for(i = 0; i < 10000; i++)
		play_sound(LA3);
	nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(LA3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(F3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
	nosound();
    
    for(i = 0; i < 10000; i++)
 	   play_sound(LA3);
 	nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(F3);
    nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(C4);
    nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(LA3);
    nosound();
    
    for(i = 0; i < 10000; i++) 
    	play_sound(E4);
    nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(E4);
    nosound();

    for(i = 0; i < 10000; i++)
    	play_sound(E4); 
	nosound();
    
    for(i = 0; i < 10000; i++)
	    play_sound(F4);
	nosound();
    
    for(i = 0; i < 10000; i++)
	    play_sound(C4);
	nosound();
    
    for(i = 0; i < 10000; i++)
 	   play_sound(Ab3); 
 	nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(F3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Ab4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(G4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Gb4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(E4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    {
    	play_sound(F4);
    		nosound();
    	play_sound(F4);
    		nosound();
	}
    
    for(i = 0; i < 10000; i++)
    	play_sound(Bb3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Eb4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(D4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Db4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(B3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    {
	    play_sound(C4);
	    	nosound();
    	play_sound(C4);
    		nosound();
    }
    
    for(i = 0; i < 10000; i++)
    	play_sound(F3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Ab3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(F3);
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
	nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(E4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
   		play_sound(LA4); 
   	nosound();
    
    for(i = 0; i < 10000; i++)
   		play_sound(Ab4); 
   	nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(G4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Gb4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
	    play_sound(E4); 
	nosound();
    
    for(i = 0; i < 10000; i++)
	{
		play_sound(F4); 
		nosound();
	    play_sound(F4); 
	    nosound();
    }

    for(i = 0; i < 10000; i++)
	    play_sound(Bb3); 
	nosound();
    
    for(i = 0; i < 10000; i++)
	    play_sound(Eb4); 
	nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(D4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Db4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(B3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    {
    	play_sound(C4); 
    	nosound();
  		play_sound(C4); 
  		nosound();//PAUSE QUASI FINE RIGA
    }

    for(i = 0; i < 10000; i++)
    	play_sound(F3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(Ab3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(F3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();
    
    for(i = 0; i < 10000; i++)
   		play_sound(F3); 
   	nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(C4); 
    nosound();
    
    for(i = 0; i < 10000; i++)
    	play_sound(LA3); 
    nosound();
}

/************************	ONLY SOUND THINGS	*************************************/
//sample mario sound
	// for(z = 0; z < 500000; z++)
	// 	play_sound(1000);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(100);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(900);

	// for(z = 0; z < 50000; z++)
	// 	play_sound(200);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(800);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(300);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(700);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(400);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(600);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(500);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(500);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(1000);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(400);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(300);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(300);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(200);


	 //sample bday song


	//jingle bell song
	// for(a = 0; a < 7; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(E3);
	// }

	// for(z = 0; z < 500000; z++)
	// 	play_sound(G3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(C3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E3);	
	// for(a = 0; a < 5; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(F3);
	// }

	// for(a = 0; a < 4; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(E3);
	// }
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);	
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E3);	
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);	
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G3);		
	// for(a = 0; a < 7; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(E3);
	// }
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(C3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);		
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E3);
	// for(a = 0; a < 5; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(F3);
	// }
	// for(a = 0; a < 3; a++)
	// {
	// 	for(z = 0; z < 500000; z++)
	// 		play_sound(E3);
	// }
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(F3);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(D3);	
	// for(z = 0; z < 500000; z++)
	// 	play_sound(C3);



	//star war songs
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(B1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G1);	
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(B1);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G1);

	// for(z = 0; z < 500000; z++)
	// 	play_sound(29.14);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(29.14);							
	// for(z = 0; z < 500000; z++)
	// 	play_sound(29.14);


	// for(z = 0; z < 500000; z++)
	// 	play_sound(B2);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G2);							
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E2);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(B2);
	// for(z = 0; z < 500000; z++)
	// 	play_sound(G2);							
	// for(z = 0; z < 500000; z++)
	// 	play_sound(E2);	




// D2 C2 B1 A1 A2 A2 E1 G1 G2 G2 C2 A1 A2	

/************************	ONLY SOUND THINGS	*************************************/

