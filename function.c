#include "main.h"

void gotoxy(unsigned int x, unsigned int y)
{
	if ((x == 0) || (y == 0))
	{
		perror("gotoxy with zero");
		return;
	}

	printf("\033[%d,%df", y, x);
	fflush(stdout);
}

void showCursor(int isShow)
{
	if (isShow)
		printf("\033[?25h");
	else
		printf("\033[?25l");
	fflush(stdout);
}
