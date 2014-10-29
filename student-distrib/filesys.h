#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"

typedef struct {
	uint32_t file_type;
	uint32_t inode_num;
	uint8_t file_name[32];
	uint8_t reserved[24]; 
}dentry_t;

typedef struct{
	uint32_t length;
}inode;

void init_filesys(uint32_t *bootblock);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif
