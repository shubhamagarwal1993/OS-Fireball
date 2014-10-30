#include "filesys.h"
#include "lib.h"


static bootblock_t boot_block;

// uint32_t num_dir_entries;
// uint32_t num_inodes;
// uint32_t num_data_blocks;
// uint32_t* bootblock;

void init_filesys(const uint32_t *bootblockptr)
{
	printf("init file system\n");
	// printf("bootblockptr: 0x%x\n", bootblockptr);

	// printf("Bootblockptr[0]: 0x%x\n", *(bootblockptr));
	// printf("Bootblock: 0x%x\n", bootblock);

	boot_block.num_dir_entries = *(bootblockptr);
	boot_block.num_inodes = *(bootblockptr + 1);
	boot_block.num_data_blocks = *(bootblockptr + 2);

	printf("num_dir_entries: 0x%x\nnum_inodes: 0x%x\nnum_data_blocks 0x%x\n", boot_block.num_dir_entries, boot_block.num_inodes, boot_block.num_data_blocks);

	fill_boot_block();

	uint8_t name[32] = "hello";
	dentry_t* blank;
	int32_t ret_val;
	ret_val = read_dentry_by_name(name, blank);

	printf("Return value = %d\n", ret_val);

	int i;
	printf("Name: ");
	for(i = 0; i < strlen(name); i++)
		printf("%c", name[i]);
	printf("\n");

	printf("Blank: ");
	for(i = 0; i < strlen(name); i++)
		printf("%c", blank->file_name[i]);
	printf("\n");

}

void fopen()
{

}


void fclose()
{
	
}


void fill_boot_block()
{
	
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	// int i;
	// int len = strlen((int8_t*)fname);
	// uint8_t dir_en_name_buf[33] = { 0 };

	// for(i = 1; i <= boot_block.num_dir_entries; i++)
	// {
	// 	dentry_t* cur_entry = (dentry_t*)(bootblock + i * 16);
	// 	uint8_t* dir_en_name = (uint8_t*)cur_entry;
	// 	strncpy((int8_t*)dir_en_name_buf, (int8_t*)dir_en_name, 32);
	// 	//int dir_en_name_len = strlen(dir_en_name_buf);
	// 	if(len != strlen((int8_t*)dir_en_name_buf))
	// 		continue;
	// 	if(strncmp((int8_t*)fname, (int8_t*)dir_en_name_buf, len) != 0)
	// 		continue;

	// 	// Copy directory entry
	// 	strncpy((int8_t*)dentry->file_name, (int8_t*)dir_en_name_buf, 32);
	// 	dentry->file_type = cur_entry->file_type;
	// 	dentry->inode_num = cur_entry->inode_num;

	// 	// Copy reserved bytes
	// 	int j;
	// 	for(j = 0; j < 24; j++)
	// 		dentry->reserved[j] = cur_entry->reserved[j];
	// 	return 0;
	// }

	// return -1;		

}

// File types are 0 for a file giving user-level access to the real-time clock (RTC)
// 1 for the directory,
// 2 for a regular file.



// The index node number is only meaningful for
// regular files and should be ignored for the RTC and directory types.
// Each regular file is described by an index node that specifies the file’s size in bytes and the data blocks that make up
// the file. Each block contains 4 kB; only those blocks necessary to contain the specified size need be valid, so be careful
// not to read and make use of block numbers that lie beyond those necessary to contain the file data.







int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	if(index < 0)
		return -1;

	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	if((inode <0) && (inode >15))
		return -1;
	
	

	return 0;
}
