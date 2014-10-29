#include "filesys.h"
#include "lib.h"

uint32_t num_dir_entries;
uint32_t num_inodes;
uint32_t num_data_blocks;

void init_filesys(uint32_t *bootblock)
{
	printf("init file system");
	num_dir_entries = *bootblock;
	num_inodes = *(bootblock + 1);
	num_data_blocks = *(bootblock + 2);
}

void fopen()
{

}


void fclose()
{
	
}


int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	if(fname<0)
		return -1;



	return 0;		



// File types are 0 for a file giving user-level access to the real-time clock (RTC)
// 1 for the directory,
// 2 for a regular file.



// The index node number is only meaningful for
// regular files and should be ignored for the RTC and directory types.
// Each regular file is described by an index node that specifies the file’s size in bytes and the data blocks that make up
// the file. Each block contains 4 kB; only those blocks necessary to contain the specified size need be valid, so be careful
// not to read and make use of block numbers that lie beyond those necessary to contain the file data.




}


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
