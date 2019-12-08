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

int _getch(void)
{
	struct termios o, n;
	int ch;

 	tcgetattr(STDIN_FILENO, &o);
 	n = o;
 	n.c_lflag &= ~( ICANON | ECHO );

 	tcsetattr(STDIN_FILENO, TCSANOW, &n);
 	ch = getchar();

 	tcsetattr(STDIN_FILENO, TCSANOW, &o);
 	return ch;
}

void exiting(void)
{
	showCursor(1);
}
