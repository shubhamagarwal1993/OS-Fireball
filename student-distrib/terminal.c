#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "buffer.h"
#include "i8259.h"


volatile int enter_flag = 0;
volatile int length = 0;
volatile int to_print;
node* curr_buff;
node* screens[3] = {0,0,0};
int screen_num = 0;
int buf_x;
node buffer[NUM_COLS*NUM_ROWS];

// void init_terminal()
// {
// 	node buffer[NUM_COLS*NUM_ROWS];
// 	curr_buff = buffer;
// 	screens[screen_num] = curr_buff;
// 	screen_num++;
// 	//return curr_buff;
// }

/* 
 * terminal_open()
 *   DESCRIPTION: Opens the terminal and allows typing to it
 *   INPUTS: the integer signal received from the keyboard interrupt.
 *   OUTPUTS: --
 *   RETURN VALUE: --
 *   SIDE EFFECTS:
 */
node* terminal_open()
{
	curr_buff = buffer;
	screens[screen_num] = curr_buff;
	screen_num++;
	return curr_buff;
	//terminal_write(buffer, 1);
}

/* 
 * terminal_read()
 *   DESCRIPTION: Read counter number of indexes from the buffer
 *   INPUTS: the buffer, the required counter
 *   OUTPUTS: string representing the required information that was read
 *   RETURN VALUE: string
 *   SIDE EFFECTS: --
 */
uint32_t terminal_read(uint8_t* buf, int counter)
{
	sti();
	int i = 0;
	for (i = 0; i < strlen(buf); ++i)
	{
		buf[i] = '\0';
	}
	node* buffer = curr_buff;
	to_print = inb(0x60);
	char* output;
	char out[counter];
	//char * fault = "Invalid read";
	int line_count;
	int temp_buf_x = buf_x;


	while(1)
	{
		if(length == 127)
			break;
		if(enter_press == 1)
			break;
		else line_count = pass_count();
	}
		enter_press = 0;
	int y = pass_y();
	// int test_count = pass_count();
	int index = 0;
	if(line_count >= 80)
		index = (y-2)*80;
	else index = (y-1)*80;
	if(enter_flag == 1)
		index = (y-1)*80;
	//if( (counter > 0) && (counter <= 128) )
	//{
	int j = 0;
	for(i = index + temp_buf_x; i < index + temp_buf_x + line_count; i++, j++)
	{
		length++; 
		out[j] = buffer[i].mo;
		// if( (length == 127) || (to_print == 0x1C) || (to_print == 0x0E))
		// {
		// 	counter = length - 1;
		// 	break;
		// }
		if(buffer[i+1].mo == '\n')
		{
			//index = y - 1;
			// out[i - index - temp_buf_x] = '\n';
			output = out;
			break;
		}
	}
	output = out;
	//}
	//else 
	//{
	//	output = fault;
	//}
	int yudodis;
	for(yudodis = 0; yudodis < length; yudodis++)
	{
		buf[yudodis] = output[yudodis];
	}
	buf[yudodis] = '\0';
	if(counter > 128)
		return 128;
	else return counter;
}

/* 
 * terminal_write()
 *   DESCRIPTION: Writes to the terminal and prints when required
 *   INPUTS: the buffer, the required counter, the integer signal received from the keyboard interrupt.
 *   OUTPUTS: number of bytes written to the screen.
 *   RETURN VALUE: int32_t
 *   SIDE EFFECTS: --
 */
int32_t terminal_write(uint8_t* buf, int counter)
{
	//to_print = inb(0x60);
	//kbd_int_handler();
	int i;
	char* print_this = (char*)buf;

	for(i = 0; i < strlen(print_this); i++)
		put_cout(print_this[i]);
	printb(pass_buff());
	return counter;

	//previous code below:

	//int test_count = pass_count();
	//char* test = "lol, yiss!";
	//if(test_count == 50) cout(test);
	//if( (test_count % counter == 0) || (to_print == 0x1C) || (to_print == 0x0E))
	//	printb(buf);
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
	screen_num--;
	screens[screen_num] = 0;
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
	// int test_count = pass_count();
	// if((test_count != 127)&&(key != 0x1C))
	// 	terminal_write(buf, test_count);
	// //	printb(buf);
	// else
	// {
	// 	if(key == 0x1C)
	// 		enter_flag = 1;
	// 	disable_irq(1);
	// 	int count = 127;
	// 	char* halwai = terminal_read(buf, 127);
	// 	//reset_buf(buf);
	// 	if(enter_flag)
	// 		count = length;
	// 	int i = 0;
		
	// 	while((i < 127) || (halwai[i] != '\n'))
	// 	{
	// 		if (i == 0)
	// 			new_line(buf);
	// 		setb(buf, halwai[i]);
	// 		if (i == 79)
	// 			new_line(buf);
	// 		i++;
	// 	}
	// 	for(i = 0; i < count; i++)
	// 	{
	// 		if (i == 0)
	// 			new_line(buf);
	// 		setb(buf, halwai[i]);
	// 		if (i == 79)
	// 			new_line(buf);
	// 	}
	// 	// new_line(buf);
	// 	printb(buf);
	// }
	// test_count++;
}

/* 
 * pass_buff()
 *   DESCRIPTION: Helper function. Used to provide other functions
 * 				  an access to the buffer.
 *   INPUTS: -- 
 *   OUTPUTS: the buffer
 *   RETURN VALUE: node *
 *   SIDE EFFECTS:
 */
node* pass_buff()
{
	return curr_buff;
}
