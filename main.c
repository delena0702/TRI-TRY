#include "main.h"

void childHandler(int signum);
void playGame();

pid_t child[3];

int main(void)
{
	printf("\033[H\033[J");
	
	while(1)
	{
		printf("\033[%dd\033[%dG%s",1 ,1, "1. Start Game");
		printf("\033[%dd\033[%dG%s",2 ,1, "2. Exit Game");

		int input = _getch();

		switch(input)
		{
			case '1':
				playGame();
				break;

			case '2':
				return 1;
				break;

		}
	}


}

void childHandler(int signum)
{
	unsigned int i;

	for (i=0;i<3;i++)
		kill(child[i],SIGKILL);
	printf("\033[H\033[J");
	printf("Game Over");
	exit(0);
}

void playGame()
{
	int fd[3][2];
	int i;

	struct sigaction sa;

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
			atexit(exiting);
			showCursor(0);

			sa.sa_handler = &childHandler;
			sa.sa_flags = 0;

			if((sigemptyset(&sa.sa_mask) == -1) || (sigaction(SIGCHLD, &sa, NULL) == -1))
				perror("Failed to install SIGALRM signal handler");

			// 종료 시까지 대기
			while (1)
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
		}
		break;
	}

	exit(0);
	return ;

}
