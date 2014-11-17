/* paging.h
 * vim:ts=4 noexpandtab
 */


#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define VIDEO_MEM 	0xB8000
#define KERNEL_MEM 	0x400000

#define PD_align	0x400000
#define PT_align	0x1000

/* Number of pages in the Page directory */
#define NUM_PDE 1024
#define NUM_PTE 1024

#define PGRM_IMG	0x08048000
#define PAGE1		0x800000
#define PAGE2		0xC00000

#ifndef ASM

// A page directory element
typedef union PDE {
		uint32_t val;
		struct {
			uint32_t present : 1;
			uint32_t read_write : 1;
			uint32_t user_super : 1;
			uint32_t write_through : 1;
			uint32_t cache_disabled : 1;
			uint32_t accessed : 1;
			uint32_t reserved : 1;
			uint32_t page_size : 1;
			uint32_t global : 1;
			uint32_t avail : 3;
			uint32_t page_table_base : 20;
		}__attribute__((packed));
} PDE_t;

// A page table element
typedef union PTE {
	uint32_t val;
	struct {
		uint32_t present : 1;
		uint32_t read_write : 1;
		uint32_t user_super : 1;
		uint32_t write_through : 1;
		uint32_t cache_disabled : 1;
		uint32_t accessed : 1;
		uint32_t dirty : 1;
		uint32_t page_table_attribute : 1;
		uint32_t global : 1;
		uint32_t avail : 3;
		uint32_t page_base : 20;
	}__attribute__((packed)); 	
} PTE_t;

/* Sets page base address in PDE for 4MB pages */
#define SET_PDE_4MB_PAGE(str, addr) 									\
do { 																	\
	str.page_table_base = (((uint32_t)(addr) & 0xFFC00000) >> 12 );		\
	str.page_size = 1; 													\
	str.accessed = 0; 													\
	str.present = 1; 													\
} while(0)

/* Sets page table address in PDE for 4KB page tables */
#define SET_PDE_4KB_PT(str, addr) 										\
do { 																	\
	str.page_table_base = (((uint32_t)(addr) & 0xFFFFF000) >> 12);		\
	str.page_size = 0; 													\
	str.accessed = 0; 													\
	str.present = 1; 													\
} while(0)

/* Sets page base address in PTE */
#define SET_PTE(str, addr) 												\
do { 																	\
	str.page_base = (((uint32_t)(addr) & 0xFFFFF000) >> 12);			\
	str.accessed = 0; 													\
	str.present = 1; 													\
} while(0)


/* Turns on paging by setting the CR0[31] bit
 * Outputs: None
 * Inputs: None
 * Clobbers eax
 */
#define set_PG()                    			\
do {                                   			\
	asm volatile("movl %%cr0, %%eax \n\t"		\
		 		 "orl  $0x80000000, %%eax \n\t"	\
				 "movl %%eax, %%cr0"			\
				 :								\
				 :								\
				 : "eax", "memory"				\
				 );       						\
} while(0)	

/* Enables 4 MB pages by setting the CR4[4] bit
 * Outputs: None
 * Inputs: None
 * Clobbers eax
 */
#define set_PSE()                    			\
do {                                   			\
	asm volatile("movl %%cr4, %%eax \n\t"		\
		 		 "orl  $0x00000010, %%eax \n\t"	\
				 "movl %%eax, %%cr4"			\
				 :								\
				 :								\
				 : "eax", "memory"				\
				 );       						\
} while(0)	

/* Enables global pages by setting the CR4[7] bit
 * Outputs: None
 * Inputs: None
 * Clobbers eax
 */
#define set_PGE()                    			\
do {                                   			\
	asm volatile("movl %%cr4, %%eax \n\t"		\
		 		 "orl  $0x00000080, %%eax \n\t"	\
				 "movl %%eax, %%cr4"			\
				 :								\
				 :								\
				 : "eax", "memory"				\
				 );       						\
} while(0)	

/* Puts a pointer to our page directory in CR3 
 * Outputs: None
 * Inputs: PD_addr -- The 4MB aligned Page directory address 
 * Clobbers eax
 */
#define set_PDBR(PD_addr)                   	\
do {                                   			\
	asm volatile("movl %0, %%eax \n\t"			\
				 "movl %%eax, %%cr3"			\
				 : 								\
				 : "r" (PD_addr)				\
				 : "eax", "memory"				\
				 );       						\
} while(0)

#define INVLPG(addr)										\
do{															\
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");	\
} while(0)


/* Global */

extern void init_paging();
extern void test_paging();
extern uint32_t task_mem_init();

/* Local functions */

void init_PD();
void init_VIDEO_PT();
uint32_t get_PDE_offset(uint32_t addr);
uint32_t get_PTE_offset(uint32_t addr);
uint32_t get_Page_offset(uint32_t addr);
void map_4MB_page(uint32_t vir_addr, uint32_t phys_addr, 
				  uint32_t user_super, uint32_t read_write);

#endif /* ASM */

#endif /* PAGING_H */
