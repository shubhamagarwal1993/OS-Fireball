#include "filesys.h"
#include "lib.h"



static bootblock_t* boot_block;
static inode * index_nodes;
static dentry_t temp_dentry;
static data_block data_blocks;

void init_filesys(const uint32_t *bootblockptr)
{
	clear();
	printf("init file system\n");

	boot_block = (bootblock_t*)bootblockptr;
	index_nodes = (inode*)(bootblockptr + 1); 

	printf("num_dir_entries: 0x%x\nnum_inodes: 0x%x\nnum_data_blocks 0x%x\n", boot_block->num_dir_entries, boot_block->num_inodes, boot_block->num_data_blocks);

	uint8_t name[32] = "hello";
	dentry_t blank;
	int32_t ret_val;


	// Test read_dentry_by_index

	//ret_val = read_dentry_by_index(27, &blank);


	// Test read_dentry_by_name
	/*
	ret_val = read_dentry_by_name(name, &blank);

	int i;
	printf("Name: ");
	for(i = 0; i < strlen((int8_t*)name); i++)
	printf("%c", name[i]);
	printf("\n");

	printf("Blank: ");
	for(i = 0; i < strlen((int8_t*)name); i++)
	printf("%c", blank.file_name[i]);
	printf("\n");
	*/



	printf("Return value = %d\n", ret_val);

}

void fopen()
{
	return;
}


void fclose()
{
	return;
}


int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	int i;
	uint32_t typed_len = strlen((int8_t*)fname);

	for(i = 0; i < boot_block->num_dir_entries; i++)
	{
		dentry_t* cur_entry;
		cur_entry = &(boot_block->dir_entries[i]);

		uint32_t entry_name_len = strlen((int8_t*)(cur_entry->file_name));

		if(entry_name_len > 32)
			entry_name_len = 32;

		if(typed_len > 32)
			typed_len = 32;
return 0; 
		if(entry_name_len != typed_len)
			continue;

		if(0 != strncmp((int8_t*)fname, (int8_t*)(cur_entry->file_name), typed_len))
			continue;

		// Copy directory entry
		strncpy((int8_t*)dentry->file_name, (int8_t*)cur_entry->file_name, 32);
		dentry->file_type = cur_entry->file_type;
		dentry->inode_num = cur_entry->inode_num;

		// Copy reserved bytes
		int j;
		for(j = 0; j < 24; j++)
			dentry->reserved[j] = cur_entry->reserved[j];
		return 0;
	}

	return -1;		

}


//fill dentry(second arg) with the file name, file type, and inode number for the file, then return 0. 
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	//check if less than 0 and greater than max
	if((index < 0) || (index >= boot_block->num_inodes))
		return -1;

	int valid_inode = 0;
	int i;
	uint32_t temp_index;

	for(i = 0; i < boot_block->num_inodes; i++)
	{
		temp_index = boot_block->dir_entries[i].inode_num;

		if(index != temp_index)
			continue;
		else						// Copy directory entry
		{
			int j;
			for(j = 0; j < 32; j++)
				dentry->file_name[j] = boot_block->dir_entries[i].file_name[j];
			
				dentry->file_type = boot_block->dir_entries[i].file_type;
		 		
		 		// Copy reserved bytes
			for(j = 0; j < 24; j++)
				dentry->reserved[j] = boot_block->dir_entries[i].reserved[j];	

			valid_inode = 1;
		}	

	}	

	if(valid_inode == 0)
		return -1;
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	//if inode is invalid then return -1
	if((inode <0) && (inode >15))							//what would the max be
		return -1;	
	
	

	return 0;
}
