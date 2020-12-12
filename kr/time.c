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

const int max_size = 1024;
const int sh_size = 4096;
const int sem_num = 2;
#define CHILD 0

int p(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = -1;
	if(semop(semid, &ops, 1) < 0)
	{
		perror("Decrement error\n");
		exit(errno);
	}
	return 0;
}

int v(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = 1;
	if(semop(semid, &ops, 1) < 0)
	{
		perror("Increment error\n");
		exit(errno);
	}
	return 0;
}

int main()
{
	int id = shmget(IPC_PRIVATE, sh_size, 0700 | IPC_CREAT);
	check(id);
	char* sh_mem = (char*)shmat(id, NULL, 0);
	if(sh_mem == NULL || (void*)sh_mem == (void*)-1)
	{
		perror("shmat error");
		return errno;
	}

	int semid = semget(IPC_PRIVATE, 1, 0700 | IPC_CREAT);
	check(semid);
	
	// structures measuring the time 
	struct timespec begin;
	struct timespec end;
	
	pid_t pid = fork();
	check(pid);
	if(pid == 0) // child
	{
		clock_gettime(CLOCK_MONOTONIC, &begin);
		sprintf(sh_mem, "%ld", (long)begin.tv_nsec);
		v(semid, CHILD); // CHILD == 1

		return 0;
	}
	else // parent
	{
		p(semid, CHILD);
		clock_gettime(CLOCK_MONOTONIC, &end);
		long time_nano = end.tv_nsec - atoi(sh_mem);
		//long ms = time_nano / 1e6;
		printf("working time is %ld ns\n", time_nano);
	}
	
	// waiting for the end of the child process
	wait(NULL);
	
	check(semctl(semid, sem_num, IPC_RMID));
	shmdt(sh_mem);
	return 0;
}

