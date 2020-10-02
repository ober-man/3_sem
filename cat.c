#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <fcntl.h>

const int maxsize = 1000;

//---------------------------------------------------------------------
//! Function "cat" concatenates files and prints on the standard output
//!
//!@param <...text...> list of files to print
//!
//!@note "cat" without parameters prints all you write on the console
//---------------------------------------------------------------------
int main(int argc, char* argv[])
{
	char* buffer = (char*)calloc(maxsize, sizeof(char));
	if(argc == 1)
	{
		//int size = sizeof(buffer);
		while(read(0, buffer, sizeof(char)) != 0)
			while(write(1, buffer, sizeof(char)) < sizeof(char))
				;
	}
	else
	{
		for(int i = 1; i < argc; ++i)
		{
			int fd = open(argv[i], O_RDONLY);
			if(fd < 0)
			{
				printf("No such file\n");
				exit(-1);
			}
			while(read(fd, buffer, sizeof(char)) != 0)
				while(write(1, buffer, sizeof(char)) < sizeof(char))
	                                ;
			close(fd);
		}	
	}
	free(buffer);
	return 0;
}
