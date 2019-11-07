#include "main.h"

void letterGame(int fd[])
{
	char ch;
	while(1)
	{
		read(fd[0], &ch, 1);

		printf("Oh It's %c\n", ch);
	}
}
