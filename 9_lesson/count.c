#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<errno.h>
#include<pthread.h>

struct argument
{
	long long num;
	int threads;
};

void* func(int num);
void* dumb(void* args);
void* naive(void* args);
void* wide(void* args);
void* optimal(void* args);

pthread_mutex_t mutex;
long long count = 0;

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		printf("Too less arguments\n");
		return -1;
	}
	long long n = atoll(argv[1]); // counting number
	int m = atoi(argv[2]); // number of threads
	int a = atoi(argv[3]); // number of algorythm
	if(a < 1 || a > 4)
	{
		printf("Wrong algorythm\n");
		return -1;
	}
	
	pthread_t* ptid = (pthread_t*)malloc(m * sizeof(pthread_t));
	pthread_mutex_init(&mutex, NULL);
	int check;
	
	struct argument args = {n, m};
	for(int i = 0; i < m; i++)
		if(pthread_create(ptid + i, NULL, func(a), (void*)&args) != 0)
		{
			perror("Create error:");
			return errno;
		}
	
	for(int i = 0; i < m; ++i)
		if(pthread_join(ptid[i], NULL) != 0)
		{
			perror("Join error:");
			return errno;
		}
	printf("%Ld\n", count);
	
	free(ptid);
	pthread_mutex_destroy(&mutex);
	return 0;
}

void* func(int num)
{
	switch(num)
	{
		case 1:
			return dumb;
		case 2:
			return naive;
		case 3:
			return wide;
		case 4:
			return optimal;
	}
}

void* dumb(void* args)
{
	struct argument* arg = (struct argument*)args;
	long long number = arg->num/arg->threads;
	for(long long i = 0; i < number; ++i)
		++count;
	return NULL;
}

void* naive(void* args)
{
	struct argument* arg = (struct argument*)args;
	long long number = arg->num/arg->threads;
	for(long long i = 0; i < number; ++i)
	{
		pthread_mutex_lock(&mutex);
		++count;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* wide(void* args)
{
	struct argument* arg = (struct argument*)args;
	long long number = arg->num/arg->threads;
	
	pthread_mutex_lock(&mutex);
	for(long long i = 0; i < number; ++i)
		++count;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

void* optimal(void* args)
{
	struct argument* arg = (struct argument*)args;
	long long number = arg->num/arg->threads;
	long long var = 0;
	for(long long i = 0; i < number; ++i)
		++var;
		
	pthread_mutex_lock(&mutex);
	count += var;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

