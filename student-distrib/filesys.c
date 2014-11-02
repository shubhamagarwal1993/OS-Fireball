#include "filesys.h"
#include "lib.h"
#include "keyboard.h"

static bootblock_t* boot_block;
static inode_t* index_nodes;
static data_block_t* data_blocks;


<<<<<<< .mine
static bootblock_t* boot_block;
static inode * index_nodes;
static dentry_t temp_dentry;
static data_block data_blocks;









void init_filesys(const uint32_t *bootblockptr)
=======
void init_filesys(const uint8_t *bootblockptr)
>>>>>>> .r15203
{
	boot_block = (bootblock_t*)bootblockptr;
	index_nodes = (inode_t*)(bootblockptr + BLOCKSIZE);
	data_blocks = (data_block_t*)(bootblockptr + (BLOCKSIZE * (boot_block->num_inodes + 1))); 

	//test_filesys();

}

/* read_dentry_by_name */
/********************************************************************
*int32_t															*
*read_dentry_by_name()												*
*	Inputs:			filename, the dentry to copy to 				*
*	Outputs:		writes dentry of filename to given dentry 		*
*	Return Value:	return 0 if file found & copied properly		*
*	Function: Should check for file existance and copy to dentry 	*
********************************************************************/

<<<<<<< .mine
	// Test read_dentry_by_index

	//ret_val = read_dentry_by_index(27, &blank);


	//Test read_dentry_by_name
	ret_val = read_dentry_by_name(name, &blank);
	

=======
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
>>>>>>> .r15203
	int i;

<<<<<<< .mine
	printf("Blank: ");
	for(i = 0; i < strlen((int8_t*)name); i++)
	printf("%c", blank.file_name[i]);
	printf("\n");
	
=======
	// Check for invalid pointers
	if(fname == NULL)
		return -1;
>>>>>>> .r15203

	if(dentry == NULL)
		return -1;


	uint32_t typed_len = strlen((int8_t*)fname);

	if(typed_len > 31)
		typed_len = 31;

	for(i = 0; i < boot_block->num_dir_entries; i++)
	{
		dentry_t* cur_entry;
		cur_entry = &(boot_block->dir_entries[i]);

		uint32_t entry_name_len = strlen((int8_t*)(cur_entry->file_name));

		// File name is allocated 32B, all characters after are ignored
		if(entry_name_len > 31)
			entry_name_len = 31;

<<<<<<< .mine
		if(typed_len > 32)
			typed_len = 32;

=======
>>>>>>> .r15203
		if(entry_name_len != typed_len)
			continue;

		if(0 != strncmp((int8_t*)fname, (int8_t*)(cur_entry->file_name), entry_name_len))
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

	// is not on the file system
	return -1;		

}


/* read_dentry_by_index */
/********************************************************************
*int32_t															*
*read_dentry_by_index()												*
*	Inputs:			index of file, the dentry to copy to			*
*	Outputs:		writes dentry of file with index to given dentry*
*	Return Value:	return 0 if file found & copied properly		*
*	Function: Should check for file existance and copy to dentry 	*
********************************************************************/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	// Check for invalid index
	if(index >= boot_block->num_dir_entries)
		return -1;

	dentry_t* cur_entry = &(boot_block->dir_entries[index]);

	// Copy the directory entry
	strncpy((int8_t*)dentry->file_name, (int8_t*)cur_entry->file_name, 32);
	dentry->file_type = cur_entry->file_type;
	dentry->inode_num = cur_entry->inode_num;
	// Copy reserved bytes
	int j;
	for(j = 0; j < 24; j++)
		dentry->reserved[j] = cur_entry->reserved[j];

	return 0;
}


/* read_data */
/************************************************************************************
*int32_t																			*
*read_data()																		*
*	Inputs:			inode #, offset from , buffer to copy to, # of bytes from offset*
*	Outputs:		writes dentry of filename to given dentry 						*
*	Return Value:	return 0 indicates end of file is reached 						*
*	Function: Should check for file existance and copy to buffer 					*
************************************************************************************/
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

<<<<<<< .mine
		if(index != temp_index)
			continue;

		else						// Copy directory entry
=======
		// Find a pointer to the current data and copy_len for this block
		data_start = (uint8_t*)(data_blocks + cur_data_block);
		if(i == 0)
>>>>>>> .r15203
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


/**************************************************
 For the handin, you must have some test/wrapper code that given a filename, a buffer, and a
buffer length, will read data from the given file into the buffer.
*****************************************************/
void test_filesys()
{
	//clear();

	// Print filesystem data
	printf("num_dir_entries: 0x%x\nnum_inodes: 0x%x\nnum_data_blocks 0x%x\n", 
			boot_block->num_dir_entries, boot_block->num_inodes, boot_block->num_data_blocks);
	printf("boot block address: 0x%x\nindex nodes address: 0x%x\ndata blocks address: 0x%x\n", 
			boot_block, index_nodes, data_blocks);
	/*
	*/

	// Test read_dentry_by_name
	/*
	dentry_t dentry;
	uint8_t name[33] = "verylargetxtwithverylongname.txt";
	printf("Return value = %d\n", read_dentry_by_name(name, &dentry));
	*/

	// Test read_dentry_by_index
	/*
	dentry_t dentry;
	printf("Return value = %d\n", read_dentry_by_index(3, &dentry));
	*/

	// Print filled dentry (read_dentry_by_name or read_dentry_by_index)
	//printf("Filled dentry: \n");
	/*
	int i;
	printf("Filled dentry: \n");
	printf("Name: %s\n", dentry.file_name);
	printf("Type: %d\n", dentry.file_type);
	printf("Inode #: %d\n", dentry.inode_num);
	printf("Reserved: 0x");
	for(i = 0; i < 24; i++)
		printf("%x", dentry.reserved[i]);
	printf("\n");
	*/


	// Test read_data
	/*
	uint8_t buf[6000] = { 0 };
	printf("Return value = %d\n", read_data(0x10, 000, buf, 6000));
	int j;
	for(j = 0; j < 6000; j++)
		printf("%c", buf[j]);
	printf("\n");
	*/

}


uint32_t file_open()
{
	return 0;
}

uint32_t file_read(const uint8_t* fname, uint32_t offset, uint8_t* buf, uint32_t length)
{
<<<<<<< .mine
	//check if less than 0 and greater than max
	if((inode < 0) || (inode >= boot_block->num_inodes))
		return -1;
=======
	dentry_t* dentry;
	read_dentry_by_name(fname, dentry);
	return read_data((dentry->inode_num), offset, buf, length);
}
>>>>>>> .r15203

<<<<<<< .mine
	int valid_inode = 0;
	int i;
	uint32_t temp_inode;

	// temp_inode = inode.data_blocks[];



	// for(i = 0; i < boot_block->num_inodes; i++)
	// {
	// 	temp_index = boot_block->dir_entries[i].inode_num;

	// 	if(index != temp_index)
	// 		continue;
	// 	else						// Copy directory entry
	// 	{
	// 		int j;
	// 		for(j = 0; j < 32; j++)
	// 			dentry->file_name[j] = boot_block->dir_entries[i].file_name[j];
			
	// 			dentry->file_type = boot_block->dir_entries[i].file_type;
		 		
	// 	 		// Copy reserved bytes
	// 		for(j = 0; j < 24; j++)
	// 			dentry->reserved[j] = boot_block->dir_entries[i].reserved[j];	

	// 		valid_inode = 1;
	// 	}	

	// }	

	// if(valid_inode == 0)
	// 	return -1;
=======
uint32_t file_write()
{
	return -1;
}

uint32_t file_close()
{
>>>>>>> .r15203
	return 0;
}


uint32_t dir_open()
{
	return 0;
}
/************************************************************************
should be similar to the 'ls' source code

write a test case to show that we can read different types of files 
*************************************************************************/

uint32_t dir_read(uint32_t index, uint8_t* buf, uint32_t length)
{
	dentry_t* dentry;
	if(-1 == read_dentry_by_index(index, dentry))
		return 0;

	uint32_t name_len = (length > 32) ? 32 : length;

	strncpy((int8_t*)buf, (int8_t*)dentry->file_name, name_len);

	return length;
}

uint32_t dir_write()
{
	return -1;
}


uint32_t dir_close()
{
	return 0;
}




