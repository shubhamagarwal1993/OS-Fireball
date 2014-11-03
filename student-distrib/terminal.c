#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "buffer.h"
#include "i8259.h"

/* 
 * terminal_open()
 *   DESCRIPTION: Opens the terminal and allows typing to it
 *   INPUTS: the integer signal received from the keyboard interrupt.
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS:
 */
void terminal_open(int key)
{
	node* buf = pass_buff();
	terminal_write(buf, 1, key);
}

/* 
 * terminal_read()
 *   DESCRIPTION: Read counter number of indexes from the buffer
 *   INPUTS: the buffer, the required counter
 *   OUTPUTS: string representing the required information that was read
 *   RETURN VALUE: string
 *   SIDE EFFECTS: --
 */
char* terminal_read(node* buf, int counter)
{
	char* output;
	char out[counter];
	char * fault = "Invalid read";
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
			out[i-index] = buf[i].mo;
		}
		output = out;
	}
	else 
	{
		output = fault;
	}
	return output;
}

/* 
 * terminal_write()
 *   DESCRIPTION: Writes to the terminal and prints when required
 *   INPUTS: the buffer, the required counter, the integer signal received from the keyboard interrupt.
 *   OUTPUTS: number of bytes written to the screen.
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: --
 */
int32_t terminal_write(node* buf, int counter, int key)
{
	int test_count = pass_count();
	//char* test = "lol, yiss!";
	//if(test_count == 50) cout(test);
	if( (test_count % counter == 0) || (key == 0x1C) || (key == 0x0E))
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

/* 
 * terminal_close()
 *   DESCRIPTION: Close terminal. Return 0.
 *   INPUTS: --
 *   OUTPUTS: 0
 *   RETURN VALUE: int
 *   SIDE EFFECTS: --
 */
int terminal_close()
{
	return 0;
}

/* 
 * test_read_write()
 *   DESCRIPTION: Custom test case that is designed to test read and write functions
 *   INPUTS: the buffer, the required counter, the integer signal received from the keyboard interrupt.
 *   OUTPUTS: number of bytes written to the screen.
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: --
 */
void test_read_write(node* buf, int key)
{
	//printf(" Hi");
	int test_count = pass_count();
	if(test_count != 127)
		terminal_write(buf, test_count, key);
	//	printb(buf);
	else
	{
		disable_irq(1);
		char* halwai = terminal_read(buf, 127);
		//reset_buf(buf);
		int i = 0;
		for(i = 0; i < 127; i++)
		{
			if (i == 0)
				new_line(buf);
			setb(buf, halwai[i]);
			if (i == 79)
				new_line(buf);
		}
		// new_line(buf);
		printb(buf);
	}
	test_count++;
}
