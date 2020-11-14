#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<errno.h>
#include<time.h>

int ship_num = 0;
int first_time = 1;
const int SEM_NUM = 4;

#define DOWN 0
#define UP 1
#define EMPTY_ROAD 2
#define EMPTY_RIVER 3

void car(int num, int semid);
void ship(int num, int semid);
int p(int semid, int num);
int v(int semid, int num);

int main(int argc, char* argv[])
{
	if(argc < 2)
		return -1;
	int n = atoi(argv[1]);
	
	// Create the semaphores
	int semid = semget(IPC_PRIVATE, SEM_NUM, 0700);
	if(semid < 0)
	{
		perror("Create error\n");
		return errno;
	}
	v(semid, EMPTY_ROAD);
	v(semid, EMPTY_RIVER);
	v(semid, UP);
	
	// Creating n parallel processes-ships
        for(int i = 1; i <= n; ++i)
        {
                int pid = fork();
                if(pid == 0)
                {
                	ship(i, semid);
                       return 0;
                }
        }
        
	// Creating n parallel processes-cars
        for(int i = 1; i <= n; ++i)
        {
                int pid = fork();
                if(pid == 0)
                {
                	car(i, semid);
                       return 0;
                }
        }
        
        int status = 0;
        for(int i = 1; i <= 2*n; ++i)
                wait(&status);
       
        // Delete the semaphores
	if(semctl(semid, SEM_NUM, IPC_RMID) < 0)
	{
		perror("Delete error");
		return errno;
	}
      	return 0;
}

void car(int num, int semid)
{
	printf("Car %d move to the bridge\n", num);
	
	// Only first car might move the bridge
	if(ship_num > 2 && first_time == 1)
	{
		first_time = 0;
		p(semid, UP); // start UP = 1, it must be reduced for waiting
		v(semid, DOWN); // let the ships go 
	}
	
	// If can cross (start UP = 1) - cross
	// else wait the bridge
	p(semid, UP);
	v(semid, UP);
	
	// Critical section: bridge 1 by 1
	p(semid, EMPTY_ROAD);
	printf("Car %d crossed the bridge\n", num);
	v(semid, EMPTY_ROAD);
}

void ship(int num, int semid)
{
	printf("Ship %d move to the bridge\n", num);
	++ship_num;
	
	// Only first ship can move the bridge
	if(ship_num > 2 && first_time == 1)
	{
		first_time = 0;
		p(semid, UP); // stop the cars
		v(semid, DOWN); // let the ships go
	}
	
	// If can cross (DOWN = 1 after previous construction) - cross 
	// Waiting the bridge
	p(semid, DOWN);
	v(semid, DOWN);
	
	if(ship_num == 1) // last ship
	{
		// Critical section: river 1 by 1
		p(semid, EMPTY_RIVER);
		printf("Ship %d crossed the river\n", num);
		--ship_num;// new
		v(semid, EMPTY_RIVER);
		
		v(semid, UP);
		p(semid, DOWN);
		first_time = 1;
	}
	else
	{
		// Critical section: river 1 by 1
		p(semid, EMPTY_RIVER);
		printf("Ship %d crossed the river\n", num);
		--ship_num;// new
		v(semid, EMPTY_RIVER);
	}
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
    
