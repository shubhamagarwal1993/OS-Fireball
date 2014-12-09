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
#include "i8259.h"

/*	global variables for parse 	*/
char * args;
int space_seen = 0;
int index = 0;
uint8_t open_processes = 0;
uint32_t k_bp = 0;
int process_id = 0;
pcb_t* curr_process = NULL;
uint32_t retval;

int counter = 0;
uint32_t initial_shell = 1;

//process_attr task_attr[7];
int32_t task_queue[7] = {-1, -1, -1, -1, -1, -1, -1};
uint32_t next_available = 0;
uint32_t next_task = 0;

driver_jt_t file_jt = {file_open, file_read, file_write, file_close};
driver_jt_t directory_jt = {dir_open, dir_read, dir_write, dir_close};
driver_jt_t rtc_jt = {rtc_open, rtc_read, rtc_write, rtc_close};
driver_jt_t stdin_jt = {NULL, terminal_read, NULL, NULL};
driver_jt_t stdout_jt = {NULL, NULL, terminal_write, NULL};

int32_t execute(const uint8_t* command)
{

	// uint32_t flags;
	// cli_and_save(flags);
	pcb_t* previous_pcb = curr_process;


	char* cmd = (char*)command;
	uint8_t* fname = (uint8_t*)parse(cmd);
	get_arg((char *)command, (int)strlen(cmd));
	//cout("%s", cmd);
	/*	Looking for processes	*/
	uint8_t process_mask = MASK;
	//int i;
	//uint8_t temp = open_processes;
	int i = 0;
	for(i = 0; i < 7; i++)
	{
		if((process_mask & open_processes) == 0)
		{
			open_processes |= process_mask;
			process_id = i;
			break;
		}
		else
			process_mask >>= 1;
	}

	// if(num_processes + 1 > 2)
	// {
	// 	cout("PROCESS LIMIT EXCEEDED. NOT ALLOWED TO EXECUTE\n");
	// 	//num_processes--;
	// 	sti();
	// 	asm volatile("jmp ret_halt");	
	// }
	if(i == 7)
	{

		printf("Too many processes!\n");
		return 1;
	}

	/* Executable check */
	uint8_t elf_check[4];

	dentry_t dentry_temp;
	if(-1 == read_dentry_by_name(fname, &dentry_temp))
	{
		//restore_flags(flags);
		return -1;
	}

	if(-1 == read_data(dentry_temp.inode_num, 0, elf_check, 4))
	{
		//restore_flags(flags);
		return -1;
	}

	if(!(elf_check[0] == 0x7f && elf_check[1] == 0x45 && elf_check[2] == 0x4c && elf_check[3] == 0x46))
	{
		//restore_flags(flags);
		return -1;
	}

	/* Find the address of the file's first instruction */
	uint8_t buf_temp[4];

	if(-1 == read_data(dentry_temp.inode_num, 24, buf_temp, 4))
	{
		//restore_flags(flags);
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
		//restore_flags(flags);
		printf("Too many processes!\n");
		return 1;
	}

	if(-1 == program_load(fname, PGRM_IMG))
	{
		//restore_flags(flags);	
		return -1;
	}

	k_bp = _8MB - (_8KB)*(process_id) - 4;
	curr_process = (pcb_t *) (k_bp & 0xFFFFE000);
	// k_bp = _8MB - (_8KB)*(process_id);
	// curr_process = (pcb_t *) ((k_bp - 1) & 0xFFFFE000);
	curr_process->process_id = process_id;
	curr_process->PD_ptr = PD_ptr;
	curr_process->k_bp = k_bp;
	curr_process->k_sp = k_bp;
	
	//if(open_processes == MASK)
	if(initial_shell)
	{
		curr_process->parent_process = curr_process;
		curr_process->child_flag = 0;
		initial_shell = 0;
	}
	else
	{
		curr_process->parent_process = previous_pcb;
		curr_process->parent_process->child_flag = 1;
	}
	
	for(i = 0; i < 8; i++)
	{
		curr_process->file_fds[i].file_op = NULL;
		curr_process->file_fds[i].inode_ptr = NULL;
		curr_process->file_fds[i].file_pos = 0;
		curr_process->file_fds[i].flags = 0;
	}	

	task_queue[next_available] = process_id;
	next_available = (next_available + 1) % 7;

	stdin(0);									//kernel should automatically open stdin and stdout
	stdout(1);									//which correspond to fd 0 and 1 respectively

	// uint32_t ksp_temp;
	// uint32_t kbp_temp;
	// asm volatile("movl %%esp, %0":"=g"(ksp_temp));
	// asm volatile("movl %%ebp, %0":"=g"(kbp_temp));
	// curr_process->k_sp = ksp_temp;
	// curr_process->k_bp = kbp_temp;

	//context_switch(curr_process->process_id);
	// curr_process->esp0 = tss.esp0;
	// curr_process->ss0 = tss.ss0;
	
	task_queue[next_available] = process_id;
	next_available = (next_available + 1) % 7;
	
	tss.esp0 = curr_process->k_sp; 		// This is good code
	tss.ss0 = KERNEL_DS;

	if(previous_pcb != NULL)
	{
		asm volatile("movl %%esp, %0":"=g"(previous_pcb->k_sp));
		asm volatile("movl %%ebp, %0":"=g"(previous_pcb->k_bp));
	}

	// Run the scheduled process instead?
	jump_to_userspace(entry_addr);

	asm volatile("ret_halt:\n\t");				
	//restore_flags(flags);
	return retval;
}

//---------------------------------HALT---------------------------------------

int32_t halt(uint8_t status)
{
	// uint32_t flags;
	// cli_and_save(flags);
	pcb_t* parent_process = curr_process->parent_process;

	if(curr_process == curr_process->parent_process)
	{
		// We need to actually handle this case
		printf("YOU CAN CHECK OUT, BUT YOU CAN NEVER LEAVE...\n");
		while(1); 
		uint8_t fexec[33] = "shell";

		curr_process->process_id = 0;
		execute(fexec);
		//sti();	
		return -1;	
	}
	
	else
	{
		/*
		retval = (uint32_t)status;

		//modify open_processes to indicate that current process is not running anymore

		open_processes ^= (MASK >> curr_process->process_id);

		//clear parent process' child flag
		parent_process->child_flag = 0;

		tss.esp0 = curr_process->esp0;
		tss.ss0 = curr_process->ss0;

		uint32_t p_sp = curr_process->k_sp;
		uint32_t p_bp = curr_process->k_bp;

		asm volatile("movl %0, %%esp"::"g"(p_sp):"memory");
		asm volatile("movl %0, %%ebp"::"g"(p_bp):"memory");
		set_PDBR(curr_process->parent_process->PD_ptr);
		
		task_queue[next_task] = -1;
		next_task = (next_task + 1) % 7;
		context_switch(task_queue[next_task]);
		
		curr_process = curr_process->parent_process;
		//go back to parent's instruction pointer
		asm volatile("jmp ret_halt");
*/

		// ------------------------------------------------------------- //

		retval = (uint32_t)status;

		//modify open_processes to indicate that current process is not running anymore

		open_processes ^= (MASK >> curr_process->process_id);

		//clear parent process' child flag
		parent_process->child_flag = 0;

		tss.esp0 = parent_process->k_sp;
		//tss.ss0 = curr_process->ss0;

		set_PDBR(parent_process->PD_ptr);

		uint32_t p_sp = parent_process->k_sp;
		uint32_t p_bp = parent_process->k_bp;

		asm volatile("movl %0, %%esp"::"g"(p_sp):"memory"); 
		asm volatile("movl %0, %%ebp"::"g"(p_bp):"memory");
		
		uint32_t halting_task = (next_available - 1) % 7;
		task_queue[halting_task] = -1;
		next_available = halting_task;

		//context_switch(parent_process->process_id);
		
		curr_process = curr_process->parent_process;
		//go back to parent's instruction pointer
		asm volatile("jmp ret_halt");	
		
	}
	//restore_flags(flags);
	return 183;
}
//------------------------SCHED & CONTEXT SWITCH-----------------------------------------


void scheduling()
{
	pcb_t* next_pcb;
	uint32_t next_pid;
	uint32_t process_bp;

	uint32_t num_tasks = 0;
	uint32_t i; 
	uint32_t j = 0;

	for(i = 0; i < 7; i++)
	{
		if(task_queue[i] != -1)
			num_tasks++;
	}

	// if(task_queue[next_task + 1] == -1)
	// 	return;

	// do
	// {	
	while(j < num_tasks)
	{
		next_pid = task_queue[next_task];
		process_bp = _8MB - (_8KB)*(next_pid) - 4;
		next_pcb = (pcb_t *) (process_bp & 0xFFFFE000);
		task_queue[next_task] = -1;
		next_task = (next_task + 1) % 7;
		task_queue[next_available] = next_pid;
		next_available = (next_available + 1) % 7;
		if(next_pcb->child_flag == 0 && next_pid != curr_process->process_id)
			break;
		j++;
	} //while(j < num_tasks && (next_pcb->child_flag || next_pid == curr_process->process_id));

	if(j < num_tasks)
		context_switch(next_pid);
}

void context_switch(int new_pid)
{
	if(new_pid == -1)
	{
		send_eoi(0);
		return;
	}

	uint32_t process_bp = _8MB - (_8KB)*(new_pid) - 4;
	pcb_t* new_process = (pcb_t *) (process_bp & 0xFFFFE000);

	set_PDBR(new_process->PD_ptr);

	tss.esp0 = new_process->k_sp;
	uint32_t p_sp = new_process->k_sp;
	uint32_t p_bp = new_process->k_bp;

	process_buf = new_process->file_fds[1].file_pos;

	process_id = new_pid;
	asm volatile("movl %%esp, %0":"=g"(curr_process->k_sp));
	asm volatile("movl %%ebp, %0":"=g"(curr_process->k_bp));
	curr_process = new_process;

	asm volatile("movl %0, %%esp"::"g"(p_sp):"memory");
	asm volatile("movl %0, %%ebp"::"g"(p_bp):"memory");
	// uint32_t process_bp = _8MB - (_8KB)*(new_pid) - 4;
	// pcb_t* new_process = (pcb_t *) (process_bp & 0xFFFFE000);


	// set_PDBR(curr_process->PD_ptr);

	// tss.esp0 = new_process->esp0;
	// tss.ss0 = new_process->ss0;


	send_eoi(0);
	// sti();
	// return;

}

//---------------------------------------------------------------------------------------

/* int32_t open(const uint8_t* filename)
 *	
 *
 *
 */
int32_t open(const uint8_t* filename)
{
	// uint32_t flags;
	// cli_and_save(flags);
	
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

	//restore_flags(flags);
	return fd;
}

// ------------------------------- READ ----------------------------------------------------------------
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	// int i = 0;	
	// uint32_t flags;
	// cli_and_save(flags);

	if((fd < 0) || (fd > 7) || buf == NULL || nbytes < 0)
		return -1;

	file_t* file = &(curr_process->file_fds[fd]);

	if(file->flags == 0 || file->file_op->read == NULL)
		return -1;

	//restore_flags(flags);
	return file->file_op->read(file, buf, nbytes);
}

// ------------------------------- WRITE -------------------------------------------------------------
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	
	// uint32_t flags;
	// cli_and_save(flags);

	if((fd < 0) || (fd > 7) || buf == NULL || nbytes < 0)
		return -1;

	file_t* file = &(curr_process->file_fds[fd]);

	if(file->flags == 0 || file->file_op->write == NULL)
		return -1;

	//restore_flags(flags);
	return file->file_op->write(file, buf, nbytes);
}

// ------------------------------- CLOSE ----------------------------------------------------------------
int32_t close(int32_t fd)
{
	
	// uint32_t flags;
	// cli_and_save(flags);

	pcb_t* cur_PCB = curr_process;

	file_t* farray = cur_PCB->file_fds;

	if((fd < 2)||(fd > 7)||(farray[fd].flags == 0))
	{
		return -1;
	} 

	farray[fd].flags = 0;
	//restore_flags(flags);
	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	// uint32_t flags;
	// cli_and_save(flags);	

	uint32_t length;
	length = strlen(args);
	memset(buf, 0, nbytes);
	memcpy(buf, args, length);

	//restore_flags(flags);
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
