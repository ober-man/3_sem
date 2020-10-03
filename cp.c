#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

const int maxsize = 1000;
extern char* optarg;
extern int optind, opterr, optopt;

int main(int argc, char*argv[])
{
	if(argc < 3)
	{
		printf("Too less arguments\n");
		exit(-1);
	}
	char* optstr = "iv";
	int opt;
	while((opt = getopt(argc, argv, optstr)) != -1)
	{
		switch(opt)
		{
			case 'i':
				if(argc - 1 < optind + 1)
                                {
                                        printf("Too less arguments\n");
                                        exit(-1);
                                }
				if(open(argv[optind+1], O_WRONLY) > 0)
				{
					printf("overwrite %s?", argv[optind+1]);
					char ans;
					scanf("%c", &ans);
					if(ans == 'y')
						break;
					else
						exit(0);
				}
				//else
				break;

			case 'v':
				if(argc - 1 < optind + 1)
				{
					printf("Too less arguments\n");
					exit(-1);
				}
				printf("%s -> %s\n", argv[optind], argv[optind+1]);
				break;
			case ':':
				printf("Need argument\n");
				exit(-1);
			case '?':
				printf("Wrong option symbol\n");
				exit(-1);
			default:
				exit(-1);
		}
	}
	int fd1 = open(argv[optind], O_RDONLY);
	if(fd1 < 0)
	{
		printf("No such file or directory\n");
		exit(-1);
	}

	int fd2 = open(argv[optind+1], O_RDWR | O_CREAT, 0777);

	char* buf = (char*)calloc(maxsize, sizeof(char));
	while(read(fd1, buf, sizeof(char)) != 0)
		while(write(fd2, buf, sizeof(char)) < sizeof(char))
			;

	close(fd1);
	close(fd2);
}
