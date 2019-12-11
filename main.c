#include "main.h"

void childHandler(int signum);
void mainAlarmHandler(int signum);
void playGame();

pid_t child[3];
int isGameEnd;
unsigned int score;

int main(void)
{
	atexit(exiting);

	while(1)
	{
		printf("\033[H\033[J");
		printf("\033[%dd\033[%dG%s",1 ,1, "1. Start Game");
		printf("\033[%dd\033[%dG%s",2 ,1, "2. Exit Game\n");

		int input = _getch();

		switch(input)
		{
			case '1':
				playGame();
				break;

			case '2':
				return 0;
		}
	}
}

void childHandler(int signum)
{
	unsigned int i;

	for (i=0;i<3;i++)
		kill(child[i],SIGKILL);

	isGameEnd = 1;
}

void mainAlarmHandler(int signum)
{
	printf("\033[2d\033[60G\033[36m%d\033[0m초",score);
	score+=1;
}

void playGame()
{
	int fd[3][2];
	int i, j;

	struct sigaction sa, alarmsa;

	printf("\033[H\033[J");

	//pipe 생성
	if (pipe(fd[0]) || pipe(fd[1]) || pipe(fd[2]))
	{
		perror("pipe Error");
		exit(-1);
	}


	for (i=0; i<3; i++)
		if ((child[i] = fork()) <= 0)
			break;

	//printf("Hi I'm %d and My child is %d (i = %d)\n", getpid(), child, i);//확인 구문
	//자식은 i = 0, 1, 2 부모는 i = 3
	//각 함수로 pipe 인자를 보냄

	switch (i)
	{
	case 0:
		letterGame(fd[0]);
		exit(0);

	case 1:
		snakeGame(fd[1]);
		exit(0);

	case 2:
		hurdleGame(fd[2]);
		exit(0);

	case 3:
		{
			struct itimerval tim;
			score = 1;
			showCursor(0);

			sa.sa_handler = &childHandler;
			sa.sa_flags = 0;

			if((sigemptyset(&sa.sa_mask) == -1) || (sigaction(SIGCHLD, &sa, NULL) == -1))
				perror("Failed to install SIGALRM signal handler");

			alarmsa.sa_handler = &mainAlarmHandler;
			alarmsa.sa_flags = 0;

			if((sigemptyset(&alarmsa.sa_mask) == -1) || (sigaction(SIGALRM, &alarmsa, NULL) == -1))
				perror("Failed to install SIGALRM signal handler");
			
			tim.it_value.tv_sec = 1;
			tim.it_value.tv_usec = 0;
			tim.it_interval.tv_sec = 1;
			tim.it_interval.tv_usec = 0;
		   
			if (setitimer(ITIMER_REAL, &tim, NULL) == -1)
			{
				perror("main Setitimer Error");
			}

			isGameEnd = 0;
			// 종료 시까지 대기
			while (!isGameEnd)
			{
				int ch = _getch();
				
				switch (ch)
				{
				case 32: // Space Bar
					write(fd[2][1], &ch, 1);
					break;

				case 27: // 상하좌우
					if (_getch() == 91)
					{
						ch = _getch();

						if ((ch >= 65) && (ch <= 68))
							write(fd[1][1], &ch, 1);
					}
					break;

				default:
					// 알파벳
					if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')))
					{
						write(fd[0][1], &ch, 1);
					}
					break;
				}
			}

			for (i=0;i<3;i++)
				for (j=0;j<2;j++)
					close(fd[i][j]);

			tim.it_value.tv_sec = 0;
			tim.it_value.tv_usec = 0;
			tim.it_interval.tv_sec = 1;
			tim.it_interval.tv_usec = 0;
		   
			if (setitimer(ITIMER_REAL, &tim, NULL) == -1)
			{
				perror("main Setitimer Error2");
			}

			printf("\033[H\033[J");
			printf("Game Over\n\nYour Score is \033[36m%d\033[0m!!\n\nPlease Press \'Q\'\n(uppercase, not a lowercase \'q\')\n\n->", score);
			while (_getch() != 'Q');
		}
		break;
	}

	return;
}
