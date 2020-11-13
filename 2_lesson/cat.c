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
	int num;
	if(argc == 1)
	{
		while((num = read(0, buffer, maxsize)) > 0)
			write(1, buffer, num);
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
			while((num = read(fd, buffer, maxsize)) > 0)
				write(1, buffer, num);
			close(fd);
		}
	}
	free(buffer);
	return 0;
}
