#include "main.h"

int main(void)
{
	pid_t child;
	int fd[3][2];
	int i;

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
		//printf("1\n");
		break;

	case 1:
		snakeGame(fd[1]);
		//printf("2\n");
		break;

	case 2:
		hurdleGame(fd[2]);
		//printf("3\n");
		break;

	case 3:
		{
			char str[] = "Hi";
			write(fd[0][1],  str, 3);

			// 종료 시까지 대기
			while ((wait(NULL) != -1) || (errno == EINTR));
		}
		break;
	}

	return 0;
}
