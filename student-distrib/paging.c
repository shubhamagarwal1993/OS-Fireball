/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "lib.h"

static uint32_t task_count = 0;

static PDE_t PD[NUM_PDE] __attribute__((aligned (0x1000)));
static PDE_t PD0[NUM_PDE] __attribute__((aligned (0x1000)));
static PDE_t PD1[NUM_PDE] __attribute__((aligned (0x1000)));
static PTE_t VIDEO_PT[NUM_PTE] __attribute__((aligned (0x1000)));

static PDE_t PDE_default;
static PTE_t PTE_default;

static PDE_t* cur_PD = NULL;

void init_paging()
{
	// Initialize the Page directory
	init_PD();

	// Initialize the video page table
	init_VIDEO_PT();

	// Set PSE bit in CR4
	set_PSE();

	// Set PG bit in CR0
	set_PG();

	// Set PGE bit in CR4
	set_PGE();

	//test_paging();

}

/*
 * void init_PD()
 * 	Description: Sets all entries in the page directory
 *   Inputs: none
 *   Return Value: none
 */
void init_PD()
{
	// Fill the page directory with blank entries
	PDE_default.present = 0;
	PDE_default.read_write = 0;
	PDE_default.user_super = 0;
	PDE_default.write_through = 0;
	PDE_default.cache_disabled = 0;
	PDE_default.accessed = 0;
	PDE_default.reserved = 0;
	PDE_default.page_size = 0;
	PDE_default.global = 0;
	PDE_default.avail = 0;
	PDE_default.page_table_base = 0;

	int i;
	for(i = 0; i < NUM_PDE; i++)
	{
		PD[i] = PDE_default;
		PD0[i] = PDE_default;
		PD1[i] = PDE_default;
	}

 	cur_PD = PD;

	// Set up video memory PDE
	int video_offset;
	video_offset = get_PDE_offset(VIDEO_MEM);

	PDE_t PDE_video;
	PDE_video = PDE_default;
	SET_PDE_4KB_PT(PDE_video, VIDEO_PT);
	PD[video_offset] = PDE_video;

	// Set up kernel memory PDE
	int kernel_offset;
	kernel_offset = get_PDE_offset(KERNEL_MEM);

	PDE_t PDE_kernel;
	PDE_kernel = PDE_default;
	PDE_kernel.global = 1;
	SET_PDE_4MB_PAGE(PDE_kernel, KERNEL_MEM);
	PD[kernel_offset] = PDE_kernel;

	// Pass the address of the page directory into the PBDR
 	set_PDBR(PD);
}

/*
 * void init_VIDEO_PT()
 * 	Description: Sets all entries in the video page table
 *   Inputs: none
 *   Return Value: none
 */
void init_VIDEO_PT()
{
	// Fill the video page table with blank entries
	PTE_default.present = 0;
	PTE_default.read_write = 0;
	PTE_default.user_super = 0;
	PTE_default.write_through = 0;
	PTE_default.cache_disabled = 0;
	PTE_default.accessed = 0;
	PTE_default.dirty = 0;
	PTE_default.page_table_attribute = 0;
	PTE_default.global = 0;
	PTE_default.avail = 0;
	PTE_default.page_base = 0;

	int i;
	for(i = 0; i < NUM_PTE; i++)
	{
		VIDEO_PT[i] = PTE_default;
	}

	// Set up the video PTE
	int offset = get_PTE_offset(VIDEO_MEM);

	PTE_t PTE_video;
	PTE_video = PTE_default;
	PTE_video.global = 1;
	SET_PTE(PTE_video, VIDEO_MEM);
	VIDEO_PT[offset] = PTE_video;
}

uint32_t get_PDE_offset(uint32_t addr)
{
	return (addr & 0xFFC00000) >> 22;
}

uint32_t get_PTE_offset(uint32_t addr)
{
	return (addr & 0x003FF000) >> 12;
}

uint32_t get_Page_offset(uint32_t addr)
{
	return (addr & 0x00000FFF);
}

uint32_t task_mem_init()
{
	if(task_count == 0)
	{
		cur_PD = PD0;
		map_4MB_page(PGRM_IMG, PAGE1, 0);
	}

	else if(task_count == 1)
	{
		cur_PD = PD1;
		map_4MB_page(PGRM_IMG, PAGE2, 1);
		//set_PDBR(PD2);
	}

	else return -1;

	task_count++;

	return 0;
}

void map_4MB_page(uint32_t vir_addr, uint32_t phys_addr, uint32_t user_super)
{
	PDE_t PDE = PDE_default;

	uint32_t PD_offset;
	PD_offset = get_PDE_offset(vir_addr);

	PDE.user_super = user_super;
	SET_PDE_4MB_PAGE(PDE, phys_addr);

	cur_PD[PD_offset] = PDE;
	//INVLPG(vir_addr);
}


/*
 * void test_paging()
 * 	Description: Attempts to access specific places in memory
 *   Inputs: none
 *   Return Value: none
 */
void test_paging()
{
	// Access invalid momory
	// uint8_t* a = (uint8_t*) 0x00000000;
	// printf("\n%x\n", *a);

	// Access Kernel memory
	uint8_t* b = (uint8_t*) 0x00400000;
	printf("\n%x\n", *b);

	// Access Video memory
	// uint8_t* c = (uint8_t*) 0x000B8000;
	// printf("\n%x\n", *c);
}

