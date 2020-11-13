#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

//------------------------------------------------------------
//! This program create n parallel or series child processes
//! with using function "fork"
//! and display their PID and parent PID
//!
//!@param n Number of child processes
//!
//!@note parameter PARALLEL defines parallel type of processes
//!	you can comment its #define for series processes
//------------------------------------------------------------

int fork_series(int n);
int fork_parallel(int n);

#define PARALLEL

int main(int argc, char* argv[])
{
        if(argc < 2)
	{
		printf("Too less arguments\n");
		return 0;
	}
        
        int n = atoi(argv[1]);

	#ifdef PARALLEL
        int res_par = fork_parallel(n);
	#endif
	
	#ifndef PARALLEL
	int res_ser = fork_serial(n);
	#endif

        return 0;
}

int fork_series(int n)
{
	printf("Series processes\n");
	int pid = 0;
	printf("parent: pid = %d\n", getpid());
        for(int i = 0; i < n; ++i)
        {
                pid = fork();
                if(pid == 0)
                {
                        printf("child: pid = %d, parent = %d\n", getpid(), getppid());
                        return 0;
                }
        }
        int status = 0;
        for(int i = 0; i < n; ++i)
                wait(&status);
	return 0;
}

int fork_parallel(int n)
{
	printf("Parallel processes\n");
	int pid = 0;
	int status = 0;
	printf("parent: pid = %d\n", getpid());

	int i = 0;
	pid = fork();
	for(i = 0; i < n; ++i)
	{
		if(pid == 0)
		{
			printf("child: pid = %d, ppid = %d\n", getpid(), getppid());
			pid = fork();
		}
		else
			break;
	}
	
	for(int j = 0; j < n - i; ++j)
		wait(&status);
	return 0;
}
