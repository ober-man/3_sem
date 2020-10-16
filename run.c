#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<errno.h>
#include<time.h>

const int SIZE = 1024;

struct msgbuf
{
	long mtype;
};

int judge(int n, int q_id);
int runner(int num, int n, int q_id);

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Too less arguments\n");
		exit(-1);
	}
	int n = atoi(argv[1]);
	char* buf = (char*)calloc(SIZE, sizeof(char));
	if(setvbuf(stdout, buf, _IOLBF, SIZE) != 0)
	{
		perror("Setvbuf error\n");
		return errno;
	}
	
	// Create the queue
	int q_id = msgget(IPC_PRIVATE, 0700);
	if(q_id < 0)
	{
		perror("Msget error\n");
		return errno;
	}
	
	// Creating n parallel processes-runners
        for(int i = 1; i <= n; ++i)
        {
                int pid = fork();
                if(pid == 0)
                {
                	runner(i, n, q_id);
                       return 0;
                }
        }
        judge(n, q_id);
        
        int status = 0;
        for(int i = 1; i <= n; ++i)
                wait(&status);
       
        // Delete the queue
        if(msgctl(q_id, IPC_RMID, 0) < 0)
        {
		perror("Msgctl close error\n");
		return errno;
	}
	return 0;
}

int judge(int n, int q_id)
{
	struct timespec begin;
	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	
	printf("Judge came\n");
	struct msgbuf runner = {n+1};
	
	// Registration of the runners
	for(int i = 1; i <= n; ++i)
		msgrcv(q_id, &runner, 0, n+1, 0);
	
	printf("The race began!\n");
	// Send first runner
	runner.mtype = 1;
	msgsnd(q_id, &runner, 0, 0);
	
	// Wait n runner
	msgrcv(q_id, &runner, 0, n+1, 0);
	printf("The race ended!\n");
	
	clock_gettime(CLOCK_MONOTONIC, &end);
        long time_nano = end.tv_nsec - begin.tv_nsec;
        long ms = time_nano / 1e6;
        printf("Race time is %ld ms\n", ms);
	
	// Let the runners go
	runner.mtype = n+2;
	for(int i = 1; i <=n; ++i)
		msgsnd(q_id, &runner, 0, 0);
	
        printf("Judge left\n");
	return 0;
}

int runner(int num, int n, int q_id)
{	
	// Firstly registrate all runners
	struct msgbuf runner = {n + 1};
	printf("%d runner came\n", num);
	msgsnd(q_id, &runner, 0, 0);
	
	// Wait runner number for race
	// signals 1...n
	msgrcv(q_id, &runner, 0, num, 0);
	printf("%d runner finished\n", num);
	
	// Send message with num+1 number
	// Next runner will take it
	// and run his process
	// signals 2...n+1
	runner.mtype = num + 1;
	msgsnd(q_id, &runner, 0, 0);
	
	// Wait for signal to left the race
	msgrcv(q_id, &runner, 0, n+2, 0);
	printf("%d runner left the race\n", num);
	return 0;
}
