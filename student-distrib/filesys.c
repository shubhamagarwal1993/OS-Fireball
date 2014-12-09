#include "filesys.h"
#include "lib.h"
#include "keyboard.h"
#include "systemcalls.h"

#define MAX_NAME_LENGTH		32
#define DENTRY_RESERVED		24
#define RET_SUCCESS			0
#define RET_FAILURE			-1

bootblock_t* boot_block;
inode_t* index_nodes;
data_block_t* data_blocks;

/* init_filesys */
/********************************************************************/
/*int32_t															*/
/*init_filesys														*/
/*	Inputs:			filename, the dentry to copy to 				*/
/*	Outputs:		writes dentry of filename to given dentry 		*/
/*	Return Value:	return 0 if file found & copied properly		*/
/*					return -1 if file is not in the filesystem		*/
/*	Function: Should check for file existance and copy to dentry 	*/
/********************************************************************/
void init_filesys(const uint8_t *bootblockptr)
{
	boot_block = (bootblock_t*)bootblockptr;													//make a new pointer 
	index_nodes = (inode_t*)(bootblockptr + BLOCKSIZE);											//the inodes 				
	data_blocks = (data_block_t*)(bootblockptr + (BLOCKSIZE * (boot_block->num_inodes + 1))); 	//the data blocks
	//test_filesys();
}

/* read_dentry_by_name */
/********************************************************************
*int32_t															*
*read_dentry_by_name()												*
*	Inputs:			filename, the dentry to copy to 				*
*	Outputs:		writes dentry of filename to given dentry 		*
*	Return Value:	return 0 if file found & copied properly		*
*					return -1 if file is not in the filesystem		*
*	Function: Should check for file existance and copy to dentry 	*
********************************************************************/

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	int i;

	// Check for invalid pointers
	if(fname == NULL)																			//if filename does not exist	
		return RET_FAILURE;

	if(dentry == NULL)																			//if there is no dentry as we cannot return info
		return RET_FAILURE;

	uint32_t typed_len = strlen((int8_t*)fname);												//typecast and get the length of the filename

	if(typed_len > (MAX_NAME_LENGTH-1))																			//if our file name length crosses 32
		typed_len = (MAX_NAME_LENGTH-1);																			//limit file name to 32

	for(i = 0; i < boot_block->num_dir_entries; i++)											//check all the directory entries in the bootblock
	{
		dentry_t* cur_entry;																	//make an empty dentry
		cur_entry = &(boot_block->dir_entries[i]);													

		uint32_t entry_name_len = strlen((int8_t*)(cur_entry->file_name));						//

		// File name is allocated 32B, all characters after are ignored
		if(entry_name_len > (MAX_NAME_LENGTH-1))
			entry_name_len = (MAX_NAME_LENGTH-1);

		if(entry_name_len != typed_len)															//
			continue;

		if(0 != strncmp((int8_t*)fname, (int8_t*)(cur_entry->file_name), entry_name_len))		//
			continue;

		// Copy directory entry
		strncpy((int8_t*)dentry->file_name, (int8_t*)cur_entry->file_name, MAX_NAME_LENGTH);					//
		dentry->file_type = cur_entry->file_type;												//
		dentry->inode_num = cur_entry->inode_num;												//

		// Copy reserved bytes
		int j;
		for(j = 0; j < DENTRY_RESERVED; j++)
			dentry->reserved[j] = cur_entry->reserved[j];										//
		return RET_SUCCESS;
	}
	// is not on the file system
	return RET_FAILURE;		
}


/* read_dentry_by_index */
/********************************************************************
*int32_t															*
*read_dentry_by_index()												*
*	Inputs:			index of file, the dentry to copy to			*
*	Outputs:		writes dentry of file with index to given dentry*
*	Return Value:	return 0 if file found & copied properly		*
*					return -1 if index is invalid					*
*	Function: Should check for file existance and copy to dentry 	*
********************************************************************/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	// Check for invalid pointer
	if(dentry == NULL)
		return RET_FAILURE;

	// Check for invalid index
	if(index >= boot_block->num_dir_entries)		
		return RET_FAILURE;

	dentry_t* cur_entry = &(boot_block->dir_entries[index]);

	// Copy the directory entry
	strncpy((int8_t*)dentry->file_name, (int8_t*)cur_entry->file_name, MAX_NAME_LENGTH);						//
	dentry->file_type = cur_entry->file_type;													//
	dentry->inode_num = cur_entry->inode_num;													//
	// Copy reserved bytes
	int j;
	for(j = 0; j < DENTRY_RESERVED; j++)
		dentry->reserved[j] = cur_entry->reserved[j];											//

	return RET_SUCCESS;
}


/* read_data */
/************************************************************************************
*int32_t																			*
*read_data()																		*
*	Inputs:		inode, offset from start of file, buffer to fill, # of bytes to copy*
*	Outputs:		writes dentry of filename to given dentry 						*
*	Return Value:	number of bytes read into the buffer.							*
*					return value of 0 indicates end of file has been reached 		*
*	Function: 		Copies length bytes to buffer starting at offset bytes into 	*
*					the given inode's data blocks									*
************************************************************************************/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	// If inode is invalid then return -1
	if(inode >= boot_block->num_inodes)
		return RET_FAILURE;

	inode_t* cur_inode = &(index_nodes[inode]);

	// If offset is past the end of the file, return 0
	if(offset >= cur_inode->length)
		return RET_SUCCESS;

	// If length is invalid, reduce it to the highest valid length
	if(offset + length > cur_inode->length)
		length = cur_inode->length - offset;

	// Copy from each data block into the buffer
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
			return RET_FAILURE;

		// Find a pointer to the start of the data block in the file system
		data_start = (uint8_t*)(data_blocks + cur_data_block);

		// If this is the first data block to copy from
		if(i == 0)
		{
			// Account for the offset within this block
			data_start += (offset % BLOCKSIZE);

			// If number of bytes before end of block < length, copy only the bytes in this block
			// Otherwise copy the full length
			copy_len = ( (BLOCKSIZE - (offset % BLOCKSIZE)) < length)
					   ? (BLOCKSIZE - (offset % BLOCKSIZE))
					   : length;
		}
		// For all subsequent blocks, copy from the beginning of the block
		else 
		{
			// If the remaining length to be copied is longer than the block, copy the full block
			// Otherwise copy the length remaining
			copy_len = ((length - copied) > BLOCKSIZE)
					   ? BLOCKSIZE
					   : (length - copied); 
		}

		// Copy the data in this block
		memcpy(buf, data_start, copy_len);

		// Increment the count of bytes copied so far and the position in the buffer
		copied += copy_len;
		buf += copy_len;
	}

	return copied;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t file_open(file_t* file, const uint8_t* filename)						*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t file_open(file_t* file, const uint8_t* filename)
{
	dentry_t dentry;
	read_dentry_by_name(filename, &dentry);

	if(dentry.inode_num >= boot_block->num_inodes)
		return RET_FAILURE;

	file->file_op = &file_jt;																	//
	file->inode_ptr = (uint32_t) &(index_nodes[dentry.inode_num]);								//
	file->file_pos = 0;																			//
	file->flags = 1;																			//
	return RET_SUCCESS;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t file_read(file_t* file, uint8_t* buf, int32_t nbytes)				*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t file_read(file_t* file, uint8_t* buf, int32_t nbytes)
{
/*	
	dentry_t dentry;
	read_dentry_by_name(fname, &dentry);

	if(dentry.file_type != 2)
	{
		printf("Can only read regular file types.\n");
		return -1;
	}
	uint32_t file_len = (index_nodes[dentry.inode_num]).length;

	if(file_len <= file_position)
		return 0;

	int bytes_read = read_data((dentry.inode_num), file_position, buf, length);

	file_position += bytes_read;

	return bytes_read;
*/

	uint32_t inode_num = ((inode_t*) file->inode_ptr) - index_nodes;							//

	uint32_t bytes_read = read_data(inode_num, file->file_pos, buf, nbytes);					//

	file->file_pos += bytes_read;																//

	return bytes_read;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t file_write(file_t* file, const uint8_t* buf, int32_t nbytes)			*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t file_write(file_t* file, const uint8_t* buf, int32_t nbytes)
{
	return RET_FAILURE;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t file_close(file_t* file)												*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t file_close(file_t* file)
{
	file->file_op = NULL;
	file->inode_ptr = NULL;
	file->file_pos = 0;
	file->flags = 0;

	return RET_SUCCESS;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t dir_open(file_t* file, const uint8_t* filename)						*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t dir_open(file_t* file, const uint8_t* filename)
{
	file->file_op = &directory_jt;
	file->inode_ptr = NULL;
	file->file_pos = 0;
	file->flags = 1;

	return RET_SUCCESS;
}

// Writes directory names in order to buffer one at a time
/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t dir_read(file_t* file, uint8_t* buf, int32_t nbytes)					*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t dir_read(file_t* file, uint8_t* buf, int32_t nbytes)
{
	dentry_t dentry;

	if(file->file_pos >= boot_block->num_dir_entries)
		return RET_SUCCESS;

	if(-1 == read_dentry_by_index(file->file_pos, &dentry))
		return RET_FAILURE;

	uint32_t name_len = (nbytes > MAX_NAME_LENGTH) ? MAX_NAME_LENGTH : nbytes;

	strncpy((int8_t*)buf, (int8_t*)dentry.file_name, name_len);

	file->file_pos++;

	return strlen((int8_t *)buf);
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t dir_write(file_t* file, const uint8_t* buf, int32_t nbytes)			*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t dir_write(file_t* file, const uint8_t* buf, int32_t nbytes)
{
	return RET_FAILURE;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t dir_close(file_t* file)												*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t dir_close(file_t* file)
{
	file->file_op = NULL;																	//
	file->inode_ptr = NULL;																	//	
	file->file_pos = 0;																		//
	file->flags = 0;																		//

	return RET_SUCCESS;
}

/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* int32_t program_load(const uint8_t* fname, uint32_t addr)					*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
int32_t program_load(const uint8_t* fname, uint32_t addr)
{
	uint32_t offset = 0;
	uint8_t* buf = (uint8_t*)addr;
	uint32_t len = BLOCKSIZE;
	uint32_t ret;
	dentry_t dentry;

	if(-1 == read_dentry_by_name(fname, &dentry))											//
		return RET_FAILURE;

	if(dentry.file_type != 2)																//	
		return RET_FAILURE;	

	while(0 != (ret = read_data(dentry.inode_num, offset, buf, len))) 						//
	{
		if(ret == -1)
			return RET_FAILURE;

		offset += ret;
		buf += ret;
	}
	return RET_SUCCESS;
}

/*
 *	For the handin, you must have some test/wrapper code that given a filename, a buffer, and a
 *	buffer length, will read data from the given file into the buffer.
 */
/********************************************************************************/
// Writes length bytes of file content to buffer								*/
/* void test_filesys()															*/		
/*		Description: Reads up to length bytes of file content into buf 			*/
/*		Inputs:	buf - Holds a pointer to the file array entry for this file.	*/
/*				length - The number of bytes to read from the file 				*/
/*		Outputs: buf - The file content read 									*/
/* 		Return value: Number of bytes read into buf 							*/
/********************************************************************************/
void test_filesys()
{
	// Test file_read
	/*
	uint8_t buf[300] = { 0 };
	uint8_t* bufptr = buf;
	uint8_t name[33] = "frame1.txt";
	uint32_t len = 100;
	int k;
	
	cout("Return value: %d\n", file_read(name, bufptr, len));
	cout("Return value: %d\n", file_read(name, bufptr + len, len));
	cout("Return value: %d\n", file_read(name, bufptr + 2 * len, len));

	for(k = 0; k < 300; k++)
		cout("%c", buf[k]);
	cout("\n");
	*/



	// Test file_read on long file
	/*
	uint8_t buf[10000] = {'\0'};
	uint8_t* bufptr = buf;
	uint8_t name[33] = "verylargetxtwithverylongname.txt";
	uint32_t len = 10000;
	int k;
	
	printf("Return value: %d\n", file_read(name, bufptr, len));
	printf("Return value: %d\n", file_read(name, bufptr + len, len));
	printf("Return value: %d\n", file_read(name, bufptr + 2 * len, len));

	for(k = 0; k < (len); k++)
		printf("%c", buf[k]);
	printf("\n");
	*/
	



	// Test file_read on non-text file
	/*
	uint8_t buf[30] = { 0 };
	uint8_t* bufptr = buf;
	uint8_t name[33] = "cat";
	uint32_t len = 30;
	int k;
	
	printf("Return value: %d\n", file_read(name, bufptr, len));

	for(k = 0; k < 30; k++)
		printf("%x ", buf[k]);
	printf("\n");
	*/



	// Test dir_read
	/*
	int32_t cnt;
	uint8_t buf2[33];
	while (0 != (cnt = dir_read (buf2, 32))) {
        if (-1 == cnt) {
	        printf("directory entry read failed\n");
	        break;
	    }
	    buf2[cnt] = '\n';
	    printf("%s\n", buf2);
    }
    */


    // Lower level testing functions

	//clear();

	// Print filesystem data
	/*
	printf("num_dir_entries: 0x%x\nnum_inodes: 0x%x\nnum_data_blocks 0x%x\n", 
			boot_block->num_dir_entries, boot_block->num_inodes, boot_block->num_data_blocks);
	printf("boot block address: 0x%x\nindex nodes address: 0x%x\ndata blocks address: 0x%x\n", 
			boot_block, index_nodes, data_blocks);
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
	uint8_t buf[300] = { 0 };
	int j;

	int ret_val = read_data(0x0D, 0, buf, 300);
	printf("Return value = %d\n", ret_val);

	for(j = 0; j < 300; j++)
		printf("%c", buf[j]);
	printf("\n");
	*/
}
