#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <termios.h>
#include <stdbool.h>

void showCursor(int isShow);
int _getch(void);

void letterGame(int fd[]);
void snakeGame(int fd[]);
void hurdleGame(int fd[]);

void exiting(void);
