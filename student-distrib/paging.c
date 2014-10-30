/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "lib.h"

static PDE_t PD[NUM_PDE] __attribute__((aligned (0x400000)));
static PTE_t VIDEO_PT[NUM_PTE] __attribute__((aligned (0x1000)));

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
	PDE_t PDE_default;
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
	}

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
	PDE_kernel.user_super = 0;
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
	PTE_t PTE_default;
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
	uint8_t* b = (uint8_t*) 0x0040E000;
	printf("\n%x\n", *b);

	// Access Video memory
	// uint8_t* c = (uint8_t*) 0x000B8000;
	// printf("\n%x\n", *c);
}


