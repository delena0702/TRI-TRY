#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

void gotoxy(unsigned int x, unsigned int y);
void showCursor(int isShow);

void letterGame(int fd[]);
void snakeGame(int fd[]);
void hurdleGame(int fd[]);
