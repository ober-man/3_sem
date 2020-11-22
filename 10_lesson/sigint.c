#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void hello(int s)
{
	printf("hello, world\n");
	signal(SIGINT, hello);
}

int main()
{
	signal(SIGINT, hello);
	while(pause())
		;
	return 0;
}
