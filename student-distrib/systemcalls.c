#include "systemcalls.h"
#include "types.h"
#include "int_handler.h"
#include "filesys.h"

int32_t execute(const uint8_t* command)
{
	char* cmd = (char*)command;
	char* filename = parse(cmd);
	uint8_t* fname = (uint8_t*)filename;

	/* Executable check */
	uint8_t elf_check[4];
	dentry_t dentry_temp;
	uint32_t inode_file = 0;
	
	int32_t read_check = read_dentry_by_name(fname, &dentry_temp);
	if(read_check == -1)
		return -1;
	
	inode_file = dentry_temp.inode_num;
	
	read_check = read_data(inode_file, 0, elf_check, 4);
	if(read_check == -1)
		return -1;

	if(elf_check[0] == 0x7f && elf_check[1] == 0x45 && elf_check[2] == 0x4c && elf_check[3] == 0x46)
		return 6;	
	return 7;

	/*		*/


}

char* parse(char* input)
{
	int len = strlen(input);
	char* output = input;
	
	if(len == 0)
	{
		output = "-1";
		return output;
	}
	else if((len == 1)&&(input[0] == ' '))
	{
		output = "-1";
		return output;
	}
	else
	{
		int i = 0;
		for(i = 0; i < len; i++)
		{
			if(input[i] == ' ') break;
			output[i] = input[i];
		}
		output[i] = '\0';
	}
	return output;
}
