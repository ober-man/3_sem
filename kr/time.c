#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
#include <fcntl.h>
#include<time.h>

#define check(cmd) \
	if((cmd) < 0) \
	{ \
		perror(#cmd); \
		exit(errno); \
	}

const int max_size = 100;
const int sh_size = 4096;
const int sem_num = 2;
#define CHILD 0

struct timespec start1, start2;
struct timespec end;

int p(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = -1;
	int status = semop(semid, &ops, 1);
	check(status);
	clock_gettime(CLOCK_MONOTONIC, &end);
	return 0;
}

int v(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = 1;
	int status = semop(semid, &ops, 1);
	check(status);
	return 0;
}

int main()
{
	int semid = semget(IPC_PRIVATE, 1, 0700 | IPC_CREAT);
	check(semid);
	
	// structures measuring the time 
	struct timespec begin;
	struct timespec end;
	
	for(int i = 0; i < max_size; ++i)
        {
                pid_t pid = fork();
                check(pid);
                if(pid == 0)
                {
			clock_gettime(CLOCK_MONOTONIC, &start1);
			v(semid, CHILD); // CHILD == 1
			clock_gettime(CLOCK_MONOTONIC, &start2);
			return 0;
                }
        }
        
	// parent
	long* buf = (long*)calloc(max_size, sizeof(long));
	for(int i = 0; i < max_size; ++i)
	{
		p(semid, CHILD); // wait for opportunity to get CHILD == 0
		buf[i] = end.tv_nsec - (start2.tv_nsec - start1.tv_nsec);
	}
	
	long sum = 0;
	for(int i = 0; i < max_size; ++i)
	{
		sum += buf[i];
		//printf("%ld\n", buf[i]);
	}
	printf("an average working time is %ld ns\n", sum/max_size);
	
	// waiting for the end of the child process
	wait(NULL);
	
	check(semctl(semid, sem_num, IPC_RMID));
	return 0;
}

