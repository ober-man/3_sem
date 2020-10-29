#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<errno.h>
#include<time.h>

const int SEM_NUM = 9;

// Semaphore BOAT contains number of places on the boat
// Initial value is 'boat_cap'
#define BOAT 0

// Semaphore LADDER2BOAT contains number of passengers on the ladder
// Initial value is 'ladder_cap'
#define LADDER2BOAT 1

// Semaphore LADDER2BEACH contains number of passengers on the ladder
// Initial value is 'ladder_cap'
#define LADDER2BEACH 2

// Semaphore BEGIN is a boolean indicator
// contains 0 if the race started
// contains 1 else
#define BEGIN 3

// Semaphore END is a boolean indicator
// contains 0 if the race ended
// contains 1 else
#define END 4

// Semaphore LAST is a boolean indicator
// contains 0 if the current race was the last
// contains 1 else
#define LAST 5

// Semaphore DOWN2BOAT is a boolean indicator
// contains 0 if the ladder is down
// contains 1 else
#define DOWN2BOAT 6

// Semaphore DOWN2BEACH is a boolean indicator
// contains 0 if the ladder is down
// contains 1 else
#define DOWN2BEACH 7

// Semaphore EMPTY contains number of passengers on the boat
// Initial value is 'boat_cap'
#define EMPTY 8

int pass(int num, int semid);
int cap(int pass_num, int boat_cap, int ladder_cap, int trip_num, int semid);
int p(int semid, int num);
int v(int semid, int num);
int z(int semid, int num);

int main(int argc, char* argv[])
{
	int pass_num = 10; // number of people
	int boat_cap = 5; // capacity of the boat
	int ladder_cap = 5; // capacity of the ladder
	int trip_num = 5; // number of the trips
	if(argc >= 2)
		pass_num = atoi(argv[1]);
	if(argc >= 3)
		boat_cap = atoi(argv[2]);
	if(argc >= 4)
		ladder_cap = atoi(argv[3]);
	if(argc >= 5)
		trip_num = atoi(argv[4]);
	boat_cap = boat_cap > pass_num ? pass_num : boat_cap;
	ladder_cap = ladder_cap > pass_num ? pass_num : ladder_cap;
	
	// Create the semaphores
	// Boat, ladder, begin, end, last
	int semid = semget(IPC_PRIVATE, SEM_NUM, 0700);
	if(semid < 0)
	{
		perror("Create error\n");
		return errno;
	}
	
	// The initial assignment
	v(semid, END);  // END = 1
	v(semid, LAST); // LAST = 1
	v(semid, DOWN2BOAT); // DOWN2BOAT = 1
	v(semid, DOWN2BEACH); // DOWN2BEACH = 1
	semctl(semid, BOAT, SETVAL, boat_cap);
	semctl(semid, EMPTY, SETVAL, boat_cap);
	semctl(semid, LADDER2BOAT, SETVAL, ladder_cap);
	semctl(semid, LADDER2BEACH, SETVAL, ladder_cap);
	
	// Creating n parallel processes-passengers
        for(int i = 1; i <= pass_num; ++i)
        {
                int pid = fork();
                if(pid == 0)
                {
                	pass(i, semid);
                       return 0;
                }
        }
        cap(pass_num, boat_cap, ladder_cap, trip_num, semid);
        
        int status = 0;
        for(int i = 1; i <= pass_num; ++i)
                wait(&status);
       
        // Delete the semaphores
        if(semctl(semid, SEM_NUM, IPC_RMID) < 0)
        {
		perror("Delete error\n");
		return errno;
	}
	return 0;
}

int pass(int num, int semid)
{
	while(1)
	{
		// Waiting for the start of trip
		z(semid, BEGIN);
		z(semid, DOWN2BOAT);
		
		if(semctl(semid, LAST, GETVAL) == 0)
			break;
			
		// Entering the ladder and the boat
		p(semid, LADDER2BOAT);
		printf("Passenger number %d enter the ladder\n", num);
		p(semid, BOAT);
		printf("Passenger number %d enter the boat\n", num);
		//v(semid, LADDER);
		
		// Waiting for the end of the trip
		z(semid, END);
		
		// Entering the ladder from the boat
		z(semid, DOWN2BEACH);
		//z(semid, LADDER);
		p(semid, LADDER2BEACH);
		printf("Passenger number %d enter the ladder\n", num);
		//v(semid, LADDER);
		
		// Exit to a beach from a ladder
		printf("Passenger number %d exit to a beach\n", num);
		v(semid, BOAT);
		p(semid, EMPTY);
		
		if(semctl(semid, LAST, GETVAL) == 0)
			break;
	}
	return 0;
}

int cap(int pass_num, int boat_cap, int ladder_cap, int trip_num, int semid)
{
	//boat_cap = boat_cap > pass_num ? pass_num : boat_cap;
	//ladder_cap = ladder_cap > pass_num ? pass_num : ladder_cap;
	for(int i = 1; i <= trip_num; ++i)
	{
		printf("\n");
		printf("Welcome on the boat, it is %d/%d trip\n", i, trip_num);
		semctl(semid, EMPTY, SETVAL, boat_cap);
		semctl(semid, LADDER2BEACH, SETVAL, ladder_cap);
		semctl(semid, LADDER2BOAT, SETVAL, ladder_cap);
		p(semid, DOWN2BOAT);
		printf("The ladder is down\n");
		
		// Waiting for filling the boat
		z(semid, BOAT);
		printf("The boat is full\n");
		
		// The trip
		v(semid, DOWN2BOAT);
		printf("The ladder is up\n");
		printf("The trip began\n");
		v(semid, BEGIN);
		printf("\n");
		printf("~~~~~~~|\\~~~~~~~~~\n");
		printf("~~~~~~~|/~~~~~~~~~\n");
		printf("~~~~___|____~~~~~~\n");
		printf("~~~~\\_BOAT_/~~~~~~\n");
		printf("~~~~~~~~~~~~~~~~~~\n");
		printf("~\\|/~~~~~~~~~~\\|/~\n");
		printf("~~|~~~~~~~~~~~~|~~\n");
		printf("__|____BEACH___|__\n");
		printf("__|____________|__\n");
		printf("\n");
		printf("The trip ended\n");
		p(semid, END);
		
		if(i == trip_num)
			p(semid, LAST);
		
		// Clear the boat
		printf("The ladder is down\n");
		p(semid, DOWN2BEACH);
		z(semid, EMPTY);
		if(i == trip_num)
		{
			printf("The End\n");
			return 0;
		}
		// Else trips continue
		v(semid, DOWN2BEACH);
		p(semid, BEGIN);
		v(semid, END);
	}
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

int z(int semid, int num)
{
	struct sembuf ops;
	ops.sem_num = num;
	ops.sem_op = 0;
	if(semop(semid, &ops, 1) < 0)
	{
		perror("Wait-for-zero error\n");
		exit(errno);
	}
	return 0;
}
