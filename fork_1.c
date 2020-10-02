#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

/// Create n parallel child processes
/// with using function 'fork'

int main(int argc, char* argv[])
{
        int pid = 0;
        int n = atoi(argv[1]);
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
