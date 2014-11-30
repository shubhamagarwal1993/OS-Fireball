#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "types.h"
#include "paging.h"

#define _128MB	0x12800000
#define _8MB	0x00800000
#define _4MB	0x00400000
#define _8KB	0x2000
#define MASK	0x80

int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
//int32_t set_handler(int32_t signum, void* handler_address);
//int32_t sigreturn(void);

/*	Helper functions for execute(const uint8_t* command)	*/
char* parse(char* input);
void get_arg(char* input, int nbytes);

int32_t empty(void);
void stdin(uint32_t fd);
void stdout(uint32_t fd);

/***********************************************************************/
/*	PROCESS CONTROL BLOCK	*/
/***********************************************************************/
//each cell of PCB

typedef struct jump_table driver_jt_t;
typedef struct file{
	driver_jt_t* file_op;		 			//pointer to file operations jump table
	uint32_t inode_ptr;					//pointer to the inode of file in file system
	uint32_t file_pos;					//keeps position of where we are in the file
	uint32_t flags;						//this flag tells us if this fd is in use
}file_t;

//PCB structure 
typedef struct pcb{
	file_t file_fds[8];			//array of open files are represented with a file array defined above
	//uint32_t fd;						//integer index into this array is called a file descriptor and this integer is how user-level programs identify the open file
	uint32_t k_bp;					//keep track of parent process' base pointer
	uint32_t k_sp;					//keep track of parent prcoess' stack pointer
	PDE_t* PD_ptr;
	uint8_t process_id;
	uint32_t esp0;
	uint32_t ss0;
	//uint32_t parent_process_id;
	uint32_t child_flag;
	struct pcb* parent_process;
}pcb_t;

// Device driver jump table
typedef int32_t (*open_fp_t)(file_t*, const uint8_t*);
typedef int32_t (*read_fp_t)(file_t*, uint8_t*, int32_t);
typedef int32_t (*write_fp_t)(file_t*, const uint8_t*, int32_t);
typedef int32_t (*close_fp_t)(file_t*);
struct jump_table {
	open_fp_t open;
	read_fp_t read;
	write_fp_t write;
	close_fp_t close;
}; // driver_jt_t


/* Puts a pointer to parent esp in ESP register 
 * Outputs: None
 * Inputs:  
 * Clobbers eax
 */
#define set_ESP(p_sp)                   		\
do {                                   			\
	asm volatile("movl %0, %%eax \n\t"			\
				 "movl %%eax, %%esp"			\
				 : 								\
				 : "r" (p_sp)					\
				 : "eax", "memory"				\
				 );       						\
} while(0)

#define set_EBP(p_bp)                   		\
do {                                   			\
	asm volatile("movl %0, %%eax \n\t"			\
				 "movl %%eax, %%ebp"			\
				 : 								\
				 : "r" (p_bp)					\
				 : "eax", "memory"				\
				 );       						\
} while(0)

#endif /* _SYSTEMCALLS_H */

