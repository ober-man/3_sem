#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<sys/wait.h>

//---------------------------------------------------------------------------
//! Family of fuctions "exec" replace the  current process with a new process
//! There was used function "execvp"
//!
//!@param <...text...> Name of the new process and its arguments
//!
//!@note This program also displays the execution time
//!      with using function "clock_gettime"
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	struct timespec begin;
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	if(argc < 2)
	{
		printf("Too less arguments\n");
		return 0;
	}
	int pid = fork();
	if(pid == 0)
	{
		int check = execvp(argv[1], (argv + 1));
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
	printf("working time is %ld ms\n", ms);

	return 0;
}
