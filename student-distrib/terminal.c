#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "buffer.h"

int test_count = 0;

void init_terminal()
{
	//printf("Hello");
	
}

void terminal_open()
{
	node* buf = pass_buff();	
	//terminal_open(buf);
	terminal_write(buf, test_count);
}

char* terminal_read(node* buf, int counter)
{
	char output[counter];
	int y = pass_y();
	int line_count = pass_count();
	
	int index = 0;
	if(line_count >= 80)
		index = (y-1)*80;
	else index = y*80;

	int i = 0;
	if( (counter > 0) && (counter <= 128) )
	{
		for(i = index; i < index+counter; i++)
		{
			output[i-index] = buf[i].mo;
		}
	}
	else output[0] = "f";
	return output;
}

int32_t terminal_write(node* buf, int counter)
{
	printb(buf);
	/*char* data;
	char* output;
	data = terminal_read(buf, 128);
	
	if(data[0] == '\n')
		return -1;

	int i = 0;
	while( (i < 128) || (data[i] == '\n') )
	{
		output[i] = data[i];
		i++;
	}
	printf("%s", output);
	return i/2;*/
	return 0;
}

int terminal_close()
{
	return 0;
}

void test_read_write(node* buf)
{
	//printf(" Hi");
	if(test_count != 128)
		terminal_write(buf, test_count);
	//	printb(buf);
	else
	{
		char* halwai = terminal_read(buf, 100);
		reset_buf(buf);
		int i = 0;
		for(i = 0; i < 100; i++)
		{
			setb(buf, halwai[i]);
			if (i == 79)
				new_line(buf);
		}
		
		printb(buf);
	}
	test_count++;
}
