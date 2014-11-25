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
uint32_t k_bp = 0;
uint32_t entry_addr = 0;
int process_id = 0;
pcb_t* curr_process;
uint32_t retval;


// struct f_ops
// {
// 	uint32_t (*open)(void);
// 	uint32_t (*read_file)(uint8_t* buf, uint32_t length, const uint8_t* fname);
// 	uint32_t (*read_dir)(uint8_t* buf, uint32_t length);
// 	uint32_t (*read_rtc)(char* buf, int count);
// 	uint32_t (*read_terminal)(uint8_t* buf, int counter);
// 	uint32_t (*write_file)(void);
// 	uint32_t (*write_dir)(void);
// 	uint32_t (*write_rtc)(char* buf, int32_t frequency);
// 	uint32_t (*write_terminal)(uint8_t* buf, uint32_t length, const uint8_t* fname);
// };

// f_ops rtc_ops = {rtc_open, NULL, NULL, rtc_read, NULL, NULL, NULL, rtc_write, NULL};
// f_ops file_ops = {file_open, file_read, NULL, NULL, NULL, file_write, NULL, NULL, NULL};
// f_ops dir_ops = {dir_open, NULL, dir_read, NULL, NULL, NULL, dir_write, NULL, NULL};
// f_ops

int32_t execute(const uint8_t* command)
{
	pcb_t* parent_pcb = curr_process;

	char* cmd = (char*)command;
	uint8_t* fname = (uint8_t*)parse(cmd);
	//getargs(command, strlen(cmd));


	/*	Looking for processes	*/
	uint8_t process_mask = MASK;
	//int i;
	uint8_t temp = open_processes;
	int i = 0;
	for(i = 1; i <= 8; i++)
	{
		temp = temp & MASK;
		if(temp == 0)
		{
			process_mask = process_mask >> (i-1);
			open_processes |= process_mask;
			process_id = i;
			break;
		}
			
		temp = temp << 1;
	}

/*
	curr_process = (pcb_t *)(0x00800000 - (0x2000)*process_id);
	curr_process->k_sp = curr_process->k_bp = parent_pcb - 4;
	curr_process->parent_process = parent_pcb;
	curr_process->process_id = process_id;
*/

	/* Executable check */
	uint8_t elf_check[4];
	file_open();
	file_read(elf_check, 4, fname);

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


	/* Set up paging */
	uint32_t parent_PD = task_mem_init();

	program_load(fname, PGRM_IMG);

	k_bp = _8MB - (_8KB)*(process_id - 1) - 4;
	curr_process = (pcb_t *) (k_bp & 0xFFFFE000);
	curr_process->process_id = process_id;
	curr_process->parent_PD = parent_PD;
	
	if(open_processes == MASK)
	{
		//curr_process->parent_process_id = 0;
		curr_process->parent_process = curr_process;
		curr_process->child_flag = 0;
	}
	else
	{
		//pcb_t* parent_process = (pcb_t *) (curr_process->parent_sp & 0xFFFFE000);
		//curr_process->parent_process_id = parent_process->process_id;
		curr_process->parent_process = parent_pcb;
		curr_process->parent_process->child_flag = 1;
	}
	
	for(i = 0; i < 8; i++)
	{
		curr_process->file_fds[i].file_op = NULL;
		curr_process->file_fds[i].file_inode = NULL;
		curr_process->file_fds[i].file_pos = 0;
		curr_process->file_fds[i].flags = 0;
	}	

	uint32_t ksp_temp;
	uint32_t kbp_temp;
	asm volatile("movl %%esp, %0":"=g"(ksp_temp));
	asm volatile("movl %%ebp, %0":"=g"(kbp_temp));
	curr_process->k_sp = ksp_temp;
	curr_process->k_bp = kbp_temp;

	tss.esp0 = _8MB - _8KB*(process_id - 1) - 4;
	//tss.esp0 = curr_process->k_sp;
	tss.ss0 = KERNEL_DS;
	//k_sp = tss.esp0;


	//open stdin and stdout
	stdin(0);									//kernel should automatically open stdin and stdout
	stdout(1);									//which correspond to fd 0 and 1 respectively


	jump_to_userspace();
	

	asm volatile("ret_halt:\n\t");				

	return retval;
}

//making a file operations jump table
uint32_t stdin_jmp_table[4] = {0, (uint32_t)terminal_read, 0, 0};													//
uint32_t stdout_jmp_table[4] = {0, 0, (uint32_t)terminal_write, 0};															//
uint32_t rtc_jmp_table[4] = {(uint32_t)rtc_open, (uint32_t)rtc_read, (uint32_t)rtc_write, (uint32_t)rtc_close};			//
uint32_t file_jmp_table[4] = {(uint32_t)file_open, (uint32_t)file_read, (uint32_t)file_write, (uint32_t)file_close};	//
uint32_t dir_jmp_table[4] = {(uint32_t)dir_open, (uint32_t)dir_read, (uint32_t)dir_write, (uint32_t)dir_close};			//

int32_t halt(uint8_t status)
{
	//pcb_t* parent_process = (pcb_t *)(0x00800000 - (0x2000)*(curr_process->parent_parent_process_id));
	pcb_t* parent_process = curr_process->parent_process;

	if(curr_process == curr_process->parent_process)
	{
		/*dentry_t dentry_temp;
		if(-1 == read_dentry_by_name((uint8_t*)"shell", &dentry_temp))
		return -1;
		uint32_t inode_num_temp = dentry_temp.inode_num;
		uint8_t buf_temp[4];
		//if the current process is the parent process
		int ret_val = read_data(inode_num_temp, 24, buf_temp, 4);
		if(-1 == ret_val)
			return -1;
		int k = 0;
		for(k = 0; k < 4; k++)
			entry_addr |= (buf_temp[k] << 8*k);

		jump_to_userspace_again(entry_addr);*/
		printf("NOT ALLOWED");
		return -1;
		
	}
	
	//else if(curr_process->child_flag == 1)
	else
	{
		retval = (uint32_t)status;
		//modify open_processes to indicate that current process is not running anymore
		int i = 0;
		uint8_t process_mask = MASK;
		for(i = 0; i < (curr_process->process_id)-1; i++)
		{
			process_mask = process_mask >> 1;
		}
		open_processes = open_processes ^ process_mask;

		//clear parent process' child flag
		parent_process->child_flag = 0;


		//load the page directory of the parent

		//set the k_sp and tss to point back to parent process' k_sp and tss
		tss.esp0 = 0x00800000 - 0x2000*(curr_process->process_id - 1) - 4;
		// k_sp = tss.esp0;

		//set kernel stack pointer and kernel base pointer
		//back to the parent's base pointer and stack pointer
		//respectively.
		uint32_t p_sp = curr_process->/*parent_process->*/k_sp;
		uint32_t p_bp = curr_process->/*parent_process->*/k_bp;
		// set_ESP(p_sp);
		// set_EBP(p_bp);

		asm volatile("movl %0, %%esp"::"g"(p_sp):"memory");
		asm volatile("movl %0, %%ebp"::"g"(p_bp):"memory");
		set_PDBR(curr_process->parent_PD);
		//return this status back to parent process

		curr_process = curr_process->parent_process;
		//go back to parent's instruction pointer
		//asm volatile("leave");
		//asm volatile("ret");
		asm volatile("jmp ret_halt");	
	}
	return 183;
}

/* int32_t open(const uint8_t* filename)
 *	
 *
 *
 */
int32_t open(const uint8_t* filename)
{
	/* Get the current process */
	pcb_t* cur_PCB = curr_process;

	file_array_t* farray = cur_PCB->file_fds;

	/* Find next open slot in the file array */
	uint32_t fd = 0;
	while(farray[fd].flags == 1)
		fd++;

	/* Check if there's space for the file and open a valid dentry*/
	dentry_t dentry;
	if(fd > 7 || -1 == read_dentry_by_name(filename, &dentry))
		return -1;

	// Fill the file array entry
	switch(dentry.file_type)
	{
		case 0: // RTC
			farray[fd].file_op = rtc_jmp_table;
			farray[fd].file_inode = NULL;
			farray[fd].file_pos = 0;
			farray[fd].flags = 1;
			rtc_open();
			break;

		case 1: // Directory
			farray[fd].file_op = dir_jmp_table;
			farray[fd].file_inode = NULL;
			farray[fd].file_pos = 0;
			farray[fd].flags = 1;
			dir_open();
			break;

		case 2:	// File
			farray[fd].file_op = file_jmp_table;
			farray[fd].file_inode = &(index_nodes[dentry.inode_num]);
			farray[fd].file_pos = 0;
			farray[fd].flags = 1;
			file_open();
			break;

		default: // Invalid type
			return -1;
	}

	// Mark that this entry of the file array is in use
	farray[fd].flags = 1;

	return fd;
}

// ------------------------------- READ ----------------------------------------------------------------
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	// int i = 0;	

	if((fd < 0)||(fd > 7)||buf == NULL)
	{
		return -1;
	} 

	// uint8_t fname;
	// uint32_t temp_inode_no = curr_process->file_fds[fd].file_inode;
	// printf("inode num = %d\n", temp_inode_no);


	// for(i = 0; i < boot_block->num_dir_entries; i++)
	// {
	// 	if(boot_block.num_inodes == temp_inode_no)
	// 	{
	// 		fname = boot_block.dir_entries[i].file_name;
	// 	}
	// }	

	// // asm volatile("pushl %0"
	// // 			 "pushl %1"
	// // 			 "pushl %2"
	// // 			 "call *%4":"g"(&fname), "g"(nbytes), "g"(buf), "g"(curr_process->file_fds[fd].file_op[1]));

// ---------------------TERMINAL READ ONLY------------------------------
	// // TBI - pass other arguments inside buf
	//  int32_t num_bytes_read;
	//  num_bytes_read = terminal_read(buf, nbytes);
//---------------------------------------------------------------------
	pcb_t* cur_PCB = curr_process;
	int32_t num_bytes_read;

	uint32_t (*fptr)(char* buf, int32_t frequency) = NULL;
	fptr = cur_PCB->file_fds[fd].file_op[1];


	num_bytes_read = fptr(buf, nbytes);

	// asm volatile("pushl %%esi\n\t"	
	// 	 "call *fptr\n\t"
	// 	: 
	// 	: "r" (fptr), "r" (buf), "r" (nbytes) 
	// 	: 
	// 	);

	// 	/* Store the return value from the read function. */
	// asm volatile("movl %%eax, %0":"=g"(num_bytes_read));
	// asm volatile("addl $16, %esp    ;");
	
	// /* Update the current file's fileposition. */
	
	return num_bytes_read;
}

// ------------------------------- WRITE -------------------------------------------------------------
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{

	// if((fd < 0)||(fd > 7)||buf == NULL)
	// {
	// 	return -1;
	// } 	
	// //int32_t num_bytes_written;
	// 
	//terminal_write((void*)buf, nbytes);
	// return 0/*num_bytes_written*/;


	pcb_t* cur_PCB = curr_process;
	int32_t num_bytes_written;

	uint32_t (*fptr)(char* buf, int32_t frequency) = NULL;
	fptr = cur_PCB->file_fds[fd].file_op[2];

	num_bytes_written = fptr(buf, nbytes);

	return num_bytes_written;
}

// ------------------------------- CLOSE ----------------------------------------------------------------
int32_t close(int32_t fd)
{
	
	if((fd < 0)||(fd > 7))
	{
		return -1;
	} 

	pcb_t* cur_PCB = curr_process;

	file_array_t* farray = cur_PCB->file_fds;

	farray[fd].flags = 0;

	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	get_arg( (char*)buf, nbytes );
	int32_t result = (int32_t*)args;
	cout("GETARGS!\n");
	cout("%s", args);
	return result;
}

int32_t vidmap(uint8_t** screen_start) 
{
	cout("LOLIDK!\n");
	return 0;
}


void stdin(uint32_t fd)
{
	curr_process->file_fds[fd].file_op = stdin_jmp_table;
	curr_process->file_fds[fd].flags = 1;
	//cout("just read terminal\n");
}
void stdout(uint32_t fd)
{
	curr_process->file_fds[fd].file_op = stdout_jmp_table;		
	curr_process->file_fds[fd].flags = 1;
	//cout("stdout comp\n");
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

void get_arg(char* input, int nbytes)
{
		args = "random str2";
		
		if(input[0] == ' ')
		{
			args = "-1";
			return;
		}

		int j = 0;
		int arg_length = 0;
		//int len = strlen(input);
		for(j = index + 1; j < nbytes; j++)
		{
			args[j - index - 1] = input[j];
			arg_length++;
		}
		args[arg_length] = '\0';
}

