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
#define PARENT 1

struct timespec end;

int p(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = -1;
	int status = semop(semid, &ops, 1);
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
	struct timespec begin;
	struct timespec end;
	
	int id = shmget(IPC_PRIVATE, sh_size, 0700 | IPC_CREAT);
	check(id);
	char* sh_mem = (char*)shmat(id, NULL, 0);
	if(sh_mem == NULL || (void*)sh_mem == (void*)-1)
	{
		perror("shmat error");
		return errno;
	}
	
	int semid = semget(IPC_PRIVATE, 2, 0700 | IPC_CREAT);
	check(semid);
	
	// Measuring context + 3 syscalls
	for(int i = 0; i < max_size; ++i)
        {
                pid_t pid = fork();
                check(pid);
                if(pid == 0)
                {
			clock_gettime(CLOCK_MONOTONIC, &begin);
			sprintf(sh_mem, "%ld", (long)begin.tv_nsec);
			v(semid, CHILD); // CHILD == 1
			return 0;
                }
        }
        
	// parent
	long* buf = (long*)calloc(max_size, sizeof(long));
	for(int i = 0; i < max_size; ++i)
	{
		p(semid, CHILD); // wait for opportunity to get CHILD == 0
		clock_gettime(CLOCK_MONOTONIC, &end);
		sh_mem[0] = '\0';
		buf[i] = end.tv_nsec - atoi(sh_mem);
		
	}
	
	// Measuring 3 syscalls
	struct timespec useless;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	
	clock_gettime(CLOCK_MONOTONIC, &useless);
	sprintf(sh_mem, "%ld", (long)begin.tv_nsec);
	v(semid, CHILD);
	p(semid, CHILD);
	clock_gettime(CLOCK_MONOTONIC, &useless);
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	long delta = end.tv_nsec - begin.tv_nsec;
	
	long sum = 0;
	for(int i = 0; i < max_size; ++i)
	{
		sum += (buf[i] - delta);
		//printf("%ld\n", buf[i]);
	}
	printf("an average working time is %ld ns\n", sum/max_size);
	
	// waiting for the end of the child process
	wait(NULL);
	
	check(semctl(semid, sem_num, IPC_RMID));
	shmdt(sh_mem);
	return 0;
}

