#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<ctype.h>

extern char* optarg;
extern int optind, opterr, optopt;
const int maxsize = 1000;

int main(int argc, char* argv[])
{
	if(argc < 2)
        {
                printf("Too less arguments\n");
                exit(-1);
        }
	struct timespec begin;
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	char* optstr = "+q";
	int opt;
	while((opt = getopt(argc, argv, optstr)) != -1)
		switch(opt)
		{
			case 'q':

			case '?':
			default:
				break;
		}
	
	int fd[2] = {};
	pipe(fd);
	int read_fd = fd[0];
	int write_fd = fd[1];

	char** vec = (char**)calloc(argc, sizeof(char*));
	for(int i = 1; i < argc; ++i)
		vec[i-1] = argv[i];
	vec[argc-1] = NULL;
	int pid = fork();
	if(pid == 0) // child
	{
		close(read_fd);
		close(1);
		dup(write_fd);
		close(write_fd);
		int check = execvp(vec[0], vec);
		if(check < 0)
		{
			printf("Couldn't do this\n");
			free(vec);
			exit(-1);
		}
	}
	int status;
        wait(&status);
        clock_gettime(CLOCK_MONOTONIC, &end);
        long time_nano = end.tv_nsec - begin.tv_nsec;
        long ms = time_nano / 1e6;

	// parent
	close(write_fd);
	int str = 0;
	int word = 0;
	char* buffer = (char*)calloc(maxsize, sizeof(char));
	while(read(read_fd, buffer, 500) > 0)
		;
	while(write(1, buffer, sizeof(buffer)) > 0)
		;
				
	fprintf(stderr, "working time is %ld ms\n", ms);

	int size = sizeof(buffer);
	int beg = 0;
	while(isspace(buffer[beg]) && beg < size)
	{
		++beg;
		continue;
	}
	int flag = 1; // 1 if word started
	for(int i = beg; i < size; ++i)
	{
		char symb = buffer[i];

		if(isspace(symb))
		{
			if(flag == 1)
			{
				flag = 0;
				++word;
			}
		}
		else
			flag = 1;

		if(symb == '\n' || symb == '\0')
			++str;
	}

	fprintf(stderr, "there were %d lines, %d words and %d byte\n", str, word, size);
	free(vec);
	free(buffer);
	return 0;
}
