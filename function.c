#include "main.h"

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
	struct termios o, n;

 	tcgetattr(STDIN_FILENO, &o);
 	n = o;
 	n.c_lflag |= ICANON | ECHO ;

 	tcsetattr(STDIN_FILENO, TCSANOW, &n);
	printf("\033[H\033[J");
	fflush(stdout);
}
