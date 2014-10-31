#include "filesys.h"
#include "lib.h"

static bootblock_t* boot_block;
static inode_t* index_nodes;
static data_block_t* data_blocks;

//static dentry_t temp_dentry;

void init_filesys(const uint8_t *bootblockptr)
{
	clear();
	printf("init file system\n");

	boot_block = (bootblock_t*)bootblockptr;
	index_nodes = (inode_t*)(bootblockptr + BLOCKSIZE);
	data_blocks = (data_block_t*)(bootblockptr + (BLOCKSIZE * (boot_block->num_inodes + 1))); 

	// printf("num_dir_entries: 0x%x\nnum_inodes: 0x%x\nnum_data_blocks 0x%x\n", boot_block->num_dir_entries, boot_block->num_inodes, boot_block->num_data_blocks);
	// printf("boot block address: 0x%x\nindex nodes address: 0x%x\ndata blocks address: 0x%x\n", boot_block, index_nodes, data_blocks);

	// uint8_t name[32] = "hello";
	// dentry_t blank;
	int32_t ret_val = -1;

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

	// Test read_dentry_by_index
	/*
	ret_val = read_dentry_by_index(27, &blank);
	*/

	// Test read_data
	/*
	uint8_t buf[400] = { 0 };
	ret_val = read_data(15, 0, buf, 400);
	int i;
	printf("\n");
	for(i = 0; i < 400; i++)
		printf("%c", buf[i]);
	printf("\n");
	*/

	// Print return value
	// printf("Return value = %d\n", ret_val);

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

		// File name is allocated 32B, all characters after are ignored
		if(entry_name_len > 32)
			entry_name_len = 32;

		if(typed_len > 32)
			typed_len = 32;

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
	// If inode is invalid then return -1
	if(inode >= boot_block->num_inodes)
		return -1;

	inode_t* cur_inode = &(index_nodes[inode]);

	// If offset is past the end of the file, return 0
	if(offset >= cur_inode->length)
		return 0;

	// If length is invalid, reduce it to the highest valid length
	if(offset + length > cur_inode->length)
		length = cur_inode->length - offset;

	// Copy from each of the inode's data blocks into the buffer's consecutive memory
	uint32_t i;
	uint32_t copied = 0;
	uint32_t cur_data_block;
	uint8_t* data_start;
	uint32_t copy_len;
	for(i = 0; copied < length; i++)
	{
		// Find the current data block and check that it is valid
		cur_data_block = cur_inode->data_block_nums[i + (offset / BLOCKSIZE)];
		if( (cur_data_block >= boot_block->num_data_blocks) || (cur_data_block < 0) )
			return -1;

		// Find a pointer to the current data and copy_len for this block
		data_start = (uint8_t*)(data_blocks + cur_data_block);
		if(i == 0)
		{
			data_start += (offset % BLOCKSIZE);
			copy_len = ( (BLOCKSIZE - (offset % BLOCKSIZE)) < length )
					   ? (BLOCKSIZE - (offset % BLOCKSIZE))
					   : length;
		}
		else 
		{
			copy_len = ((length - copied) > BLOCKSIZE)
					   ? BLOCKSIZE
					   : (length - copied); 
		}

		// Copy the data in this block
		memcpy(buf, data_start, copy_len);


		copied += copy_len;
		buf += copy_len;
	}

	return copied;
}
