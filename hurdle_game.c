#include "main.h"

void hurdleGame(int fd[])
{
	while(1)
	{
		char ch;

		read(fd[0], &ch ,1);

		if (ch== 32)
		{
			printf("와 공백이다!\n");
		}
	}
}
