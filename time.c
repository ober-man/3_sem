#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<ctype.h>
#include<getopt.h>

extern char* optarg;
extern int optind, opterr, optopt;
const int maxsize = 1024;

//-----------------------------------------------------------------------
//! Program "time" replaces the current process with a new process,
//!		   output the execution time,
//!		   and output number of printed words, strings and bytes
//!
//!@param <...text...> name of the new process and its arguments
//!
//!@note the option "-q" or "--quiet" do not output the text
//!      and only output parameters
//-----------------------------------------------------------------------
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
	
	struct option quiet;
	quiet.name = "quiet";
	quiet.has_arg = no_argument;
	quiet.flag = NULL;
	quiet.val = 'q';
	
	char* optstr = "+q";
	int opt = 0; // current option
	int q = 0; // option "q" flag
	while((opt = getopt_long(argc, argv, optstr, &quiet, NULL)) != -1)
		switch(opt)
		{
			case 'q':
				q = 1;
				break;
			case '?':
			default:
				break;
		}
	
	int fd[2] = {};
	pipe(fd);
	int read_fd = fd[0];
	int write_fd = fd[1];

	int pid = fork();
	int check = 0; // function result
	
	// child
	if(pid == 0)
	{
		close(read_fd);
		close(1);
		dup(write_fd);
		close(write_fd);
		check = execvp(argv[optind], (argv + optind));
		if(check < 0)
		{
			printf("Couldn't do this\n");
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
	int str = 0; // number of strings
	int word = 0; // number of words
	int size = 0; // number of bytes
	char* buffer = (char*)calloc(maxsize, sizeof(char));
	
	int num; // number of read symbols
	while((num = read(read_fd, buffer, maxsize)) > 0)
	{
		size += num;
		int beg = 0; // first non-space position
		while(isspace(buffer[beg]) && beg < num)
			++beg; // skip space symbols

		int flag = 1; // 1 if word started
		for(int i = beg; i < num; ++i)
		{
			char symb = buffer[i];

			if(!isspace(symb))
				flag = 1;
			else if(flag == 1)
			{
				flag = 0;
				++word;
			}

			if(symb == '\n' || symb == '\0')
				++str;
		}
		if(flag == 1) // no '\n' at the end
		{
			++word;
			++str;
		}
		
		if(!q) // there was not option "-q"
			write(1, buffer, num);
	}
	close(read_fd);

	fprintf(stderr, "working time is %ld ms\n", ms);	
	fprintf(stderr, "there were %d strings, %d words and %d bytes\n", str, word, size);
	free(buffer);
	return 0;
}
