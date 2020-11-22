#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ioctl.h>

void size(int s)
{
	struct winsize window;
	ioctl(1, TIOCGWINSZ, &window);
	printf("Column: %d\n", window.ws_col);
	printf("Rows: %d\n", window.ws_row);
	for(int i = 0; i < window.ws_col; ++i)
		putchar('*');
	for(int i = 0; i < window.ws_row - 2; ++i)
	{
		putchar('*');
		for(int j = 1; j < window.ws_col - 1; ++j)
			putchar(' ');
		putchar('*');
	}
	for(int i = 0; i < window.ws_col; ++i)
		putchar('*');
}

int main()
{
	signal(SIGWINCH, size);
	while(pause())
		;
	return 0;
}

