#include "main.h"

int main(void)
{
	pid_t child;
	int fd[3][2];
	int i;

	atexit(exiting);

	//pipe 생성
	if (pipe(fd[0]) || pipe(fd[1]) || pipe(fd[2]))
	{
		perror("pipe Error");
		exit(-1);
	}


	for (i=0; i<3; i++)
		if ((child = fork()) <= 0)
			break;

	//printf("Hi I'm %d and My child is %d (i = %d)\n", getpid(), child, i);//확인 구문

	//자식은 i = 0, 1, 2 부모는 i = 3
	
	//각 함수로 pipe 인자를 보냄
	switch (i)
	{
	case 0:
		letterGame(fd[0]);
		break;

	case 1:
		snakeGame(fd[1]);
		break;

	case 2:
		hurdleGame(fd[2]);
		break;

	case 3:
		{
			showCursor(0);
			// 종료 시까지 대기
			while (1)
			{
				int ch = getch();
				
				switch (ch)
				{
				case 32: // Space Bar
					write(fd[2][1], &ch, 1);
					break;

				case 27: // 상하좌우
					if (getch() == 91)
					{
						ch = getch();

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

					else if (ch == 10) // 임시 구문
					{
						return 0;
					}

					else
					{
						printf(">>%d<<\n", ch);
						fflush(stdout);
					}
					break;
				}
			}
		}
		break;
	}

	return 0;
}
