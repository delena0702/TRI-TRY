#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <termios.h>

void gotoxy(unsigned int x, unsigned int y);
void showCursor(int isShow);
int getch(void);

void letterGame(int fd[]);
void snakeGame(int fd[]);
void hurdleGame(int fd[]);

void exiting(void);
