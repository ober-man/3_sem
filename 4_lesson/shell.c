#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

#define check(cmd) \
	if((cmd) < 0) \
	{ \
		perror(#cmd); \
		exit(errno); \
	}

const int max_args = 10;
const int max_len = 15;
char** parse(char* line, int* num, const char* delim);

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Too less arguments\n");
		exit(-1);
	}

	// Splitting by the commands, divided by a '|'
	int cmd_num = 0;
	char** cmds = parse(argv[1], &cmd_num, "|");
	
	int next_fd = 0;
	for(int i = 0; i < cmd_num; ++i)
	{	
		// Getting options/arguments of a command
		int arg_num = 0;
		char** args = parse(cmds[i], &arg_num, " \n\t\0");
		
		int fd[2] = {};
		check(pipe(fd));
		int read_fd = fd[0];
		int write_fd = fd[1];
		
		pid_t pid = fork();
		check(pid);
		
		//child
		if(pid == 0)
		{
			
			// Read from the pipe, except the first
			if(i != 0)
			{
				check(close(0));
				check(dup(next_fd));
				close(next_fd);
			}
			check(close(read_fd));
			
			// Write to the pipe, except the last
			if(i != cmd_num - 1)
			{
				check(close(1));
				check(dup(write_fd));
			}
			check(close(write_fd));
			
			check(execvp(args[0], args));
		}
		// parent
		wait(NULL);
		free(args);
		next_fd = read_fd;
		check(close(write_fd));
	}
	free(cmds);
	return 0;
}

char** parse(char* line, int* num, const char* delim)
{
	char** str = (char**)calloc(max_args, sizeof(char*));
	int counter = 0;
	char* word = strtok(line, delim);
	while(word != NULL)
	{
		str[counter] = (char*)calloc(max_len, sizeof(char));
		strcpy(str[counter++], word);
		word = strtok(NULL, delim);
	}
	*num = counter;
	return str;
}
