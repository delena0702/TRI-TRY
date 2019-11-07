#include "main.h"

void snakeGame(int fd[])
{
	char ch;

	while (1)
	{
		read(fd[0], &ch, 1);

		switch (ch)
		{
		case 65:
			printf("Up!\n");
			break;

		case 66:
			printf("Down\n");
			break;

		case 67:
			printf("Right!\n");
			break;

		case 68:
			printf("Left!\n");
			break;
		}
	}

}
