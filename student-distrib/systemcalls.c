#include "systemcalls.h"
#include "types.h"
#include "wrapper.h"
#include "filesys.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"

/*	global variables for parse 	*/
char * args;
int space_seen = 0;
int index = 0;
uint8_t open_processes = 0;
uint32_t k_bp = 0;
int process_id = 0;
pcb_t* curr_process;
uint32_t retval;
uint32_t num_processes = 0;

driver_jt_t file_jt = {file_open, file_read, file_write, file_close};
driver_jt_t directory_jt = {dir_open, dir_read, dir_write, dir_close};
driver_jt_t rtc_jt = {rtc_open, rtc_read, rtc_write, rtc_close};
driver_jt_t stdin_jt = {NULL, terminal_read, NULL, NULL};
driver_jt_t stdout_jt = {NULL, NULL, terminal_write, NULL};

int32_t execute(const uint8_t* command)
{
	uint32_t flags;
	cli_and_save(flags);
	pcb_t* parent_pcb = curr_process;

	char* cmd = (char*)command;
	uint8_t* fname = (uint8_t*)parse(cmd);
	get_arg((char *)command, (int)strlen(cmd));
	//cout("%s", cmd);
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
			process_id = i - 1;

			break;
		}
			
		temp = temp << 1;
	}

	if(num_processes + 1 > 2)
	{
		cout("PROCESS LIMIT EXCEEDED. NOT ALLOWED TO EXECUTE\n");
		//num_processes--;
		sti();
		asm volatile("jmp ret_halt");	
	}


	/* Executable check */
	uint8_t elf_check[4];

	dentry_t dentry_temp;
	if(-1 == read_dentry_by_name(fname, &dentry_temp))
	{
		restore_flags(flags);
		return -1;
	}

	if(-1 == read_data(dentry_temp.inode_num, 0, elf_check, 4))
	{
		restore_flags(flags);
		return -1;
	}

	if(!(elf_check[0] == 0x7f && elf_check[1] == 0x45 && elf_check[2] == 0x4c && elf_check[3] == 0x46))
	{
		restore_flags(flags);
		return -1;
	}

	/* Find the address of the file's first instruction */
	uint8_t buf_temp[4];

	if(-1 == read_data(dentry_temp.inode_num, 24, buf_temp, 4))
	{
		restore_flags(flags);
		return -1;
	}
	int k = 0;
	uint32_t entry_addr = 0;
	for(k = 0; k < 4; k++)
		entry_addr |= (buf_temp[k] << 8*k);


	/* Set up paging */
	PDE_t* PD_ptr = task_mem_init(process_id);
	if(PD_ptr == NULL)
	{
		restore_flags(flags);
		return -1;
	}

	if(-1 == program_load(fname, PGRM_IMG))
	{
		restore_flags(flags);	
		return -1;
	}

	k_bp = _8MB - (_8KB)*(process_id) - 4;
	curr_process = (pcb_t *) (k_bp & 0xFFFFE000);
	curr_process->process_id = process_id;
	curr_process->PD_ptr = PD_ptr;
	
	if(open_processes == MASK)
	{
		curr_process->parent_process = curr_process;
		curr_process->child_flag = 0;
	}
	else
	{
		curr_process->parent_process = parent_pcb;
		curr_process->parent_process->child_flag = 1;
	}
	
	for(i = 0; i < 8; i++)
	{
		curr_process->file_fds[i].file_op = NULL;
		curr_process->file_fds[i].inode_ptr = NULL;
		curr_process->file_fds[i].file_pos = 0;
		curr_process->file_fds[i].flags = 0;
	}	

	uint32_t ksp_temp;
	uint32_t kbp_temp;
	asm volatile("movl %%esp, %0":"=g"(ksp_temp));
	asm volatile("movl %%ebp, %0":"=g"(kbp_temp));
	curr_process->k_sp = ksp_temp;
	curr_process->k_bp = kbp_temp;

	curr_process->esp0 = tss.esp0;
	curr_process->ss0 = tss.ss0;
	tss.esp0 = _8MB - _8KB*(process_id) - 4;
	tss.ss0 = KERNEL_DS;

	stdin(0);									//kernel should automatically open stdin and stdout
	stdout(1);									//which correspond to fd 0 and 1 respectively

	num_processes++;
	jump_to_userspace(entry_addr);
	

	asm volatile("ret_halt:\n\t");				
	restore_flags(flags);
	return retval;
}

int32_t halt(uint8_t status)
{
	uint32_t flags;
	cli_and_save(flags);
	pcb_t* parent_process = curr_process->parent_process;

	if(curr_process == curr_process->parent_process)
	{
		// We need to actually handle this case
		printf("YOU CAN CHECK OUT, BUT YOU CAN NEVER LEAVE...\n");
		while(1);
		uint8_t fexec[33] = "shell";
		num_processes = 0;
		curr_process->process_id = 0;
		execute(fexec);
		sti();	
		return -1;
		
	}
	
	else
	{
		retval = (uint32_t)status;
		//modify open_processes to indicate that current process is not running anymore
		int i = 0;
		uint8_t process_mask = MASK;
		for(i = 0; i < (curr_process->process_id); i++)
		{
			process_mask = process_mask >> 1;
		}
		open_processes = open_processes ^ process_mask;
		num_processes--;

		//clear parent process' child flag
		parent_process->child_flag = 0;

		tss.esp0 = curr_process->esp0;
		tss.ss0 = curr_process->ss0;

		uint32_t p_sp = curr_process->/*parent_process->*/k_sp;
		uint32_t p_bp = curr_process->/*parent_process->*/k_bp;

		asm volatile("movl %0, %%esp"::"g"(p_sp):"memory");
		asm volatile("movl %0, %%ebp"::"g"(p_bp):"memory");
		set_PDBR(curr_process->parent_process->PD_ptr);
		
		curr_process = curr_process->parent_process;
		//go back to parent's instruction pointer
		asm volatile("jmp ret_halt");	
	}
	restore_flags(flags);
	return 183;
}

/* int32_t open(const uint8_t* filename)
 *	
 *
 *
 */
int32_t open(const uint8_t* filename)
{
	uint32_t flags;
	cli_and_save(flags);
	
	/* Get the current process */
	pcb_t* cur_PCB = curr_process;

	file_t* farray = cur_PCB->file_fds;

	/* Find next open slot in the file array */
	uint32_t fd = 0;
	while(farray[fd].flags == 1)
		fd++;

	/* Check if there's space for the file and open a valid dentry*/
	dentry_t dentry;
	if(fd > 7 || -1 == read_dentry_by_name(filename, &dentry))
		return -1;

	file_t* cur_file = &(farray[fd]);

	// Fill the file array entry
	switch(dentry.file_type)
	{
		case 0: // RTC
			if(-1 == rtc_open(cur_file, filename))
				return -1;
			break;

		case 1: // Directory
			if(-1 == dir_open(cur_file, filename))
				return -1;
			break;

		case 2:	// File
			if(-1 == file_open(cur_file, filename))
				return -1;
			break;

		default: // Invalid type
			return -1;
	}

	restore_flags(flags);
	return fd;
}

// ------------------------------- READ ----------------------------------------------------------------
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	// int i = 0;	
	uint32_t flags;
	cli_and_save(flags);

	if((fd < 0) || (fd > 7) || buf == NULL || nbytes < 0)
		return -1;

	file_t* file = &(curr_process->file_fds[fd]);

	if(file->flags == 0 || file->file_op->read == NULL)
		return -1;

	restore_flags(flags);
	return file->file_op->read(file, buf, nbytes);
}

// ------------------------------- WRITE -------------------------------------------------------------
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	
	uint32_t flags;
	cli_and_save(flags);

	if((fd < 0) || (fd > 7) || buf == NULL || nbytes < 0)
		return -1;

	file_t* file = &(curr_process->file_fds[fd]);

	if(file->flags == 0 || file->file_op->write == NULL)
		return -1;

	restore_flags(flags);
	return file->file_op->write(file, buf, nbytes);
}

// ------------------------------- CLOSE ----------------------------------------------------------------
int32_t close(int32_t fd)
{
	
	uint32_t flags;
	cli_and_save(flags);

	pcb_t* cur_PCB = curr_process;

	file_t* farray = cur_PCB->file_fds;

	if((fd < 2)||(fd > 7)||(farray[fd].flags == 0))
	{
		return -1;
	} 

	farray[fd].flags = 0;
	restore_flags(flags);
	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	uint32_t flags;
	cli_and_save(flags);	

	//get_arg( (char*)buf, nbytes );
	uint32_t length;
	length = strlen(args);
	memset(buf, 0, nbytes);
	memcpy(buf, args, length);
	restore_flags(flags);
	return 0;
}

/* vidmap
 *	 DESCRIPTION: map the video memory to the desired location
 *		  INPUTS: screen_start - memory location
 *		 OUTPUTS: none
 *	RETURN VALUE: -1 - failure
 *				   0 - success
 *	SIDE EFFECTS: none */
int32_t
vidmap(uint8_t** screen_start)
{
	if( (uint32_t)screen_start < _128MB || (uint32_t)screen_start >= (_128MB + _4MB) )
		return -1;

	*screen_start = user_vidmap();

	return 0;
}

int32_t set_handler(int32_t signum, void* handler_address)
{
	return -1;
}

int32_t sigreturn(void)
{
	return -1;
}

void stdin(uint32_t fd)
{
	curr_process->file_fds[fd].file_op = &stdin_jt;
	curr_process->file_fds[fd].flags = 1;
	//cout("just read terminal\n");
}
void stdout(uint32_t fd)
{
	curr_process->file_fds[fd].file_op = &stdout_jt;		
	curr_process->file_fds[fd].flags = 1;
	curr_process->file_fds[fd].file_pos = screen_num;
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
			if(input[i] == ' ' || input[i] == '\n')
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
		for(j = index + 1; j < nbytes; j++)
		{
			args[j - index - 1] = input[j];
			arg_length++;
		}
		args[arg_length] = '\0';
}
