#include "systemcalls.h"
#include "types.h"
#include "wrapper.h"
#include "filesys.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"

/*	global variables for parse 	*/
char * args;
int space_seen = 0;
int index = 0;
uint8_t open_processes = 0;
uint32_t k_sp = 0;
uint32_t entry_addr = 0;
int process_id = 0;

int32_t execute(const uint8_t* command)
{
	char* cmd = (char*)command;
	uint8_t* fname = (uint8_t*)parse(cmd);
	get_arg(index, cmd);

	/* Executable check */
	uint8_t elf_check[4];
	file_open();
	file_read(fname, elf_check, 4);

	if(!(elf_check[0] == 0x7f && elf_check[1] == 0x45 && elf_check[2] == 0x4c && elf_check[3] == 0x46))
		return -1;	

	//getting the address of where to put the program
	dentry_t dentry_temp;
	if(-1 == read_dentry_by_name((uint8_t*)fname, &dentry_temp))
		return -1;
	uint32_t inode_num_temp = dentry_temp.inode_num;
	uint8_t buf_temp[4];
	int ret_val = read_data(inode_num_temp, 24, buf_temp, 4);
	if(-1 == ret_val)
		return -1;
	int k = 0;
	for(k = 0; k < 4; k++)
		entry_addr |= (buf_temp[k] << 8*k);


	cout("BEFORE PAGING\n");

	/* Set up paging */
	task_mem_init();


	//cout("AFTER PAGING_+_)_(*&)*&^R(@$*YR(R TWE(B& R^%$B&($%(@&$%\n\n\n\n\n\n\n\n");
	program_load(fname, PGRM_IMG);

	/*	Looking for processes	*/
	uint8_t process_mask = 0x80;
	//int i;
	uint8_t temp = open_processes;
	int i = 0;
	for(i = 1; i <= 8; i++)
	{
		temp = temp & 0x80;
		if(temp == 0)
		{
			process_mask = process_mask >> (i-1);
			open_processes |= process_mask;
			process_id = i;
			break;
		}
			
		temp = temp << 1;
	}

	pcb_t* curr_process = (pcb_t *)(0x00800000 - (0x2000)*process_id);

	asm volatile("movl %%esp, %0":"=g"(curr_process->parent_sp));
	asm volatile("movl %%ebp, %0":"=g"(curr_process->parent_bp));
	
	if(open_processes == 0x10)
	{
		curr_process->parent_process_id = 0;
		curr_process->child_flag = 0;
		// What else do we need??????????????????????
	}
	else
	{
		pcb_t* temp = (pcb_t *)curr_process->parent_sp;
		curr_process->parent_process_id = temp->process_id;
		temp->child_flag = 1;
	}
	
	for(i = 0; i < 8; i++)
	{
		curr_process->file_fds[i].file_op = NULL;
		curr_process->file_fds[i].file_inode = 0;
		curr_process->file_fds[i].file_pos = 0;
		curr_process->file_fds[i].flags = 0;
	}	


	tss.esp0 = 0x00800000 - 0x2000*process_id - 4;
	k_sp = tss.esp0;

	cout("\nfvkjdrhbgkjfdbgkjf 0 0 0 0 0 0 ");

	//open stdin and stdout
	//stdin(0);									//kernel should automatically open stdin and stdout
	//stdout(1);									//which correspond to fd 0 and 1 respectively

	jump_to_userspace();

	return 0;
}



//making a file operations jump table
uint32_t stdin_jmp_table[4] = {0, (uint32_t)terminal_read, 0, 0};													//
uint32_t stdout_jmp_table[4] = {0, 0, (uint32_t)cout, 0};															//
uint32_t rtc_jmp_table[4] = {(int32_t)rtc_open, (int32_t)rtc_read, (int32_t)rtc_write, (int32_t)rtc_close};			//
uint32_t file_jmp_table[4] = {(int32_t)file_open, (int32_t)file_read, (int32_t)file_write, (int32_t)file_close};	//
uint32_t dir_jmp_table[4] = {(int32_t)dir_open, (int32_t)dir_read, (int32_t)dir_write, (int32_t)dir_close};			//

int32_t open(const uint8_t* filename)
{
	return 0;
}

int32_t close(int32_t fd)
{
	return 0;
}

void stdin(uint32_t fd)
{
	pcb_t* curr_process = (pcb_t *)(0x00800000 - (0x2000)*fd);
	curr_process->file_fds[fd].file_op = stdin_jmp_table;
	curr_process->file_fds[fd].flags = 1;
	cout("just read terminal\n");
}
void stdout(uint32_t fd)
{
	pcb_t* curr_process = (pcb_t*)(0x00800000 & 0x2000);
	curr_process->file_fds[fd].file_op = stdin_jmp_table;		
	curr_process->file_fds[fd].flags = 1;
	cout("stdout comp\n");
}

/*	parse function	*/
char* parse(char* input)
{
	int len = strlen(input);
	char* output = "random str1";
	
	if(len == 0)
	{
		output = "-1";
		return output;
	}
	else if(input[0] == ' ')
	{
		output = "-1";
		return output;
	}
	else
	{
		int i = 0;
		for(i = 0; i < len; i++)
		{
			if(input[i] == ' ')
			{ 
				space_seen = 1;
				break;
			}
			output[i] = input[i];
		}
		output[i] = '\0';
		index = i;
	}
	return output;
}

void get_arg(int i, char* input)
{
		args = "random str2";
		
		if(input[0] == ' ')
		{
			args = "-1";
			return;
		}

		int j = 0;
		int arg_length = 0;
		int len = strlen(input);
		for(j = i + 1; j < len; j++)
		{
			args[j - i - 1] = input[j];
			arg_length++;
		}
		args[arg_length] = '\0';
}


int32_t halt(uint8_t status)
{
	pcb_t* curr_process = (pcb_t *)(0x00800000 - (0x2000)*process_id);

	if(curr_process->child_flag == 0)
	{
		//if the current process is the parent process, not sure what to do
	}
	
	else if(curr_process->child_flag == 1)
	{
		//modify open_processes to indicate that current process is not running anymore
		int i = 0;
		uint8_t process_mask = 0x01;
		for(i = 0; i < curr_process->parent_process_id; i++)
		{
			process_mask = process_mask << 1;
		}
		open_processes = open_processes ^ process_mask;

		//clear parent process' child flag
		pcb_t* parent_process = (pcb_t *)(0x00800000 - (0x2000)*(curr_process->parent_process_id));
		parent_process->child_flag = 0;

		//load the page directory of the parent

		//set the k_sp and tss to point back to parent process' k_sp and tss
		tss.esp0 = 0x00800000 - 0x2000*(curr_process->process_id) - 4;
		k_sp = tss.esp0;

		//set kernel stack pointer and kernel base pointer
		//back to the parent's base pointer and stack pointer
		//respectively.
		uint32_t p_sp = curr_process->parent_sp;
		uint32_t p_bp = curr_process->parent_bp;
		set_ESP(p_sp);
		set_EBP(p_bp);

		//return this status back to parent process
		asm volatile("pushl %0;"::"g"(status));
		asm volatile("popl %eax");

		//go back to parent's instruction pointer
		asm volatile("leave");
		asm volatile("ret");
			
	}
	return 183;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	cout("READ!\n");
	return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{

	cout(buf);
	printf("%s", buf);
	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	cout("GETARGS!\n");
	return 0;
}

int32_t vidmap(uint8_t** screen_start) 
{
	cout("LOLIDK!\n");
	return 0;
}
