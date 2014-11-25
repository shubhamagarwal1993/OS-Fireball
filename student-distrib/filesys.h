#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"

#define BLOCKSIZE 4096

typedef struct{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode_num;
	uint8_t reserved[24]; 
}dentry_t;

typedef struct{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t reserved[52];
	dentry_t dir_entries[63];
}bootblock_t;

typedef struct{
	uint32_t length;
	uint32_t data_block_nums[1023];
}inode_t;

typedef struct{
	uint8_t data[4096];
}data_block_t;

// Filesystem structures
extern bootblock_t* boot_block;
extern inode_t* index_nodes;
extern data_block_t* data_blocks;


extern void init_filesys(const uint8_t *bootblockptr);

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern void test_filesys();

uint32_t file_open();
uint32_t file_read(uint8_t* buf, uint32_t length, const uint8_t* fname);
uint32_t file_write();
uint32_t file_close();

uint32_t dir_open();
uint32_t dir_read(uint8_t* buf, uint32_t length);
uint32_t dir_write();
uint32_t dir_close();

uint32_t program_load(const uint8_t* fname, uint32_t addr);

#endif
