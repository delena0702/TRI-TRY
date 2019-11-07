#include "main.h"

void letterGame(int fd[])
{
	printf("%s\n",__func__);fflush(stdout);
	char buf[4] = {0, };
	int a = read(fd[0], buf, 3);
	printf("%s\n", buf);
	fflush(stdout);
}
