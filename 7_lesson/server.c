#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<errno.h>
#include <fcntl.h>

#define check(cmd) \
	if((cmd) < 0) \
	{ \
		perror(#cmd); \
		exit(errno); \
	}
	
const int max_size = 1024;
const int sh_size = 4096;
const int sem_num = 2;
#define CLIENT 0
#define SERVER 1

int p(int semid, int num);
int v(int semid, int num);

int main(int argc, char* argv[])
{
	// This key is same for the server and all clients
	int key = ftok("server", 100);
	check(key);
	
	// and because of it the memory is common
	int id = shmget(key, sh_size, 0700 | IPC_CREAT);
	check(id);
	char* sh_mem = (char*)shmat(id, NULL, 0);
	if(sh_mem == NULL || (void*)sh_mem == (void*)-1)
	{
		perror("shmat error");
		return errno;
	}
	
	// and semaphores too
	int semid = semget(key, sem_num, 0700 | IPC_CREAT);
	check(semid);
	
	printf("Server start working\n");
	while(1)
	{
		// Critical section
		v(semid, SERVER); // let the client enter
		p(semid, CLIENT); // wait for end of client's work
		printf("The client is found\n");
		printf("%s", sh_mem);
		printf("Waiting new client!\n");
	}
	
	check(semctl(semid, sem_num, IPC_RMID));
	shmdt(p);
	return 0;
}

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
