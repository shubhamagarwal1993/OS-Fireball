#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"

typedef struct{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode_num;
	uint8_t reserved[24]; 
}dentry_t;

typedef struct{
	uint32_t length;
	uint32_t data_blocks[1023];
}inode;

typedef struct{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t reserved[52];
	dentry_t dir_entries[63];
} bootblock_t;

extern void init_filesys(const uint32_t *bootblockptr);

void fill_boot_block();
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif
