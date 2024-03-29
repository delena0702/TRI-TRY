﻿#include "main.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

//정의; 맵의 크기 (토의 후 변경 가능)
#define MAP_SIZE 18

//정의; 블럭의 종류. 편의 상 정의.
#define EMPTY 0x00A0
#define SNAKE 0x00A1
#define APPLE 0x00A2
#define WALL 0x00A3

//정의; 뱀의 머리 방향. 편의 상 정의.
#define EAST 0x00B0
#define WEST 0x00B1
#define SOUTH 0x00B2
#define NORTH 0x00B3

//정의; bool 타입. C언어에 boolean 타입이 없엇음,,,?
typedef enum { FALSE, TRUE } Bool;

//정의; Pst 타입. 2차원 좌표 구조체.
typedef struct Position {
	int x;
	int y;
} Pst;

Bool IsSnakeMove;	//뱀이 시간마다 움직여야 하는데 타이머핸들러 함수에서는 변수를 넘겨받을 수 없으므로 플래그값만 전역변수로 만들어준다.

void timer_handler(int signum);	//타이머가 실행될 때마다 실행되는 함수. 여기서 뱀을 움직이는 플래그를 TRUE로 바꾼다.
// void object_print(int ch);
int ObjectPrint(int, int, int, int);

void snakeGame(int fd[])
{
	//다음 구문은 게임을 초기화한다.
	srand((int)time(NULL));

	//다음의 구문은 단위 시간마다 Signal을 발생시켜, 뱀을 규칙적으로 움직이게 하기 위한 구조체들을 정의하기 위한 구문이다.
	///SIGALRM 처리를 위한 구조체.
	struct sigaction sa;
	sa.sa_handler = &timer_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGALRM, &sa, NULL);
	
	///SIGALAM의 interval을 정의하기 위한 구조체.
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 400000;	//처음 1회에 대한 interval
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 400000;	//한 번 실행 이후에 대한 interval
	setitimer (ITIMER_REAL, &timer, NULL);	//위에서 정의한 시간마다 SIGALRM을 발생시킨다.

	//다음의 구문은 스네이크 게임을 다른 게임과 함께 출력하기 위해 위치를 조정하고자, 창 크기를 받아오는 구문이다.
	//터미널의 크기를 반각 글자 단위로 받아온다.
	struct winsize w;
   	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	///화면 크기를 출력하는 테스트용 코드.
   	//printf ("lines %d\n", w.ws_row);
    //printf ("columns %d\n", w.ws_col);

	//다음의 구문은 게임에서 사용할 변수들을 정의하기 위한 구문이다.
	int x, y;

	///0. 맵
	int map[MAP_SIZE + 2][MAP_SIZE + 2];	//맵을 표현하기 위한 2차원 배열. 벽 때문에 +2
	for(y = 0; y < MAP_SIZE + 2; y++){
		for(x = 0; x < MAP_SIZE + 2; x++){
			map[y][x] = EMPTY;
		}
	}

	for (x = 0; x < MAP_SIZE + 2; x++) {
		map[0][x] = map[x][MAP_SIZE + 1] = map[MAP_SIZE + 1][x] = map[x][0] = WALL;	//맵에 벽을 만들어준다.
	}

	int timeInterval = 0;

	///1. 뱀
	Pst snake[MAP_SIZE * MAP_SIZE] = { {0,0}, };	//뱀의 머리와 각 몸통의 위치를 저장할 배열.	0이 꼬리고 MAX가 머리다.
	int snakeLength = 0;
	int snakeLengthMAX = 3;	//처음에는 3칸으로 시작한다.
	int snakeDirection = EAST;
	int snakeDirection_OLD = EAST;
	int snakeGrowthInterval = 40;	//!! 중요. 뱀은 매 40블럭을 움직일 때마다 자람, 이 간격은 게임 시간이 지날 때마다 줄어들어야 함! 임시로 정해둔 내용,
	int snakeSpeedInterval = 80;	//60칸 움직일 때마다 뱀 속도가 증가.
	double SpeedRatio = 0.98;

	IsSnakeMove = FALSE;	//signal_handler를 받을 함수
	snake[0].x = snake[0].y = (MAP_SIZE + 2) / 2;	//시작점은 맵의 중간
	map[snake[0].y][snake[0].x] = SNAKE;
	snakeLength++;

	///2. 사과
	Pst apple = { 0,0 };	//사과의 위치를 저장할 변수.
	Bool IsAppleExist;	//사과 존재 여부.

	do {
		apple.x = (rand() % MAP_SIZE) + 1;
		apple.y = (rand() % MAP_SIZE) + 1;
	} while (map[apple.y][apple.x] != EMPTY);	//빈 칸을 찾아서
	map[apple.y][apple.x] = APPLE;				//사과를 넣어준다.
	IsAppleExist = TRUE;						//"사과는 존재한다!"

	char ch;	//입력 키를 저장하기 위한 변수.

	//맵 출력
	for(y = 0; y < MAP_SIZE + 2; y++){
		for(x = 0; x < MAP_SIZE + 2; x++){
			ObjectPrint(x, y, w.ws_col, map[y][x]);
		}
	}
	//////////////////////////////////////////
	// for(y = 0; y < MAP_SIZE + 2; y++){
	// 	for(x = 0; x < MAP_SIZE + 2; x++){
	// 		snake_move(x, y, w.ws_col);
	// 		object_print(map[y][x]);
	// 	}
	// }

	//무한 루프
	while (TRUE) {
		for(y = 0; y < MAP_SIZE + 2; y++){
			for(x = 0; x < MAP_SIZE + 2; x++){
				ObjectPrint(x, y, w.ws_col, map[y][x]);
			}
		}
		int readByte;
		ch = 0;
		readByte = read(fd[0], &ch, 1);	//부모 프로세스에서 pipe를 통해 키보드 값을 읽어옴

		if(readByte == 1) {	//값을 읽어온 경우에만
			switch (ch) {
			case 65:	//위
				if(snakeDirection_OLD != SOUTH)
					snakeDirection = NORTH;
				break;

			case 66:	//아래
				if(snakeDirection_OLD != NORTH)
					snakeDirection = SOUTH;
				break;

			case 67:	//오른쪽
				if(snakeDirection_OLD != WEST)
					snakeDirection = EAST;
				break;

			case 68:	//왼쪽
				if(snakeDirection_OLD != EAST)
					snakeDirection = WEST;
				break;
			}
		}

		else {	//값을 읽어오지 않았을 경우 (== 인터럽트 당했을 경우)
			if(IsSnakeMove == TRUE) {
				//뱀의 머리를 이동시킨다.
				switch (snakeDirection){
				case EAST: //머리가 동쪽일 경우
					snake[snakeLength].x = snake[snakeLength - 1].x + 1;
					snake[snakeLength].y = snake[snakeLength - 1].y;
					snakeDirection_OLD = EAST;
					break;
				case WEST: //서
					snake[snakeLength].x = snake[snakeLength - 1].x - 1;
					snake[snakeLength].y = snake[snakeLength - 1].y;
					snakeDirection_OLD = WEST;
					break;
				case SOUTH: //남
					snake[snakeLength].x = snake[snakeLength - 1].x;
					snake[snakeLength].y = snake[snakeLength - 1].y + 1;
					snakeDirection_OLD = SOUTH;
					break;
				case NORTH: //북
					snake[snakeLength].x = snake[snakeLength - 1].x;
					snake[snakeLength].y = snake[snakeLength - 1].y - 1;
					snakeDirection_OLD = NORTH;
					break;
				}

				//이동한 뱀의 머리가 맵에서는 어느 위치인지 파악한다.
				switch(map[snake[snakeLength].y][snake[snakeLength].x]){
				case WALL:
					return;	//게임 끝		

				case APPLE:
					snakeLengthMAX--;	//최대 뱀 길이를 감소시킨다.

					IsAppleExist = FALSE;
					//break를 씌우지 않고 넘김으로써 EMPTY와 같이 처리되도록 한다. (SNAKE 구문은 어차피 통과할 것이므로.)

				case SNAKE:
					if (IsAppleExist && !((snake[0].x == snake[snakeLength].x) && (snake[0].y == snake[snakeLength].y)))
						return;	//머리 위치와 꼬리 위치가 아니라면 게임 끝. 꼬리는 바로 움직여서 꼬리 위치는 빈 칸이 될 것이므로 종료시키지 않아도 된다.
								//IsAppleExist가 존재하는 이유는 위에서 break를 하지 않고 넘어오는데 이 조건문을 통과하기 때문에 추가시켜줌.
					//break를 씌우지 않고 넘김으로써 EMPTY와 같이 처리되도록 한다.

				case EMPTY:
					map[snake[snakeLength].y][snake[snakeLength].x] = SNAKE;	// 현재 머리 값을 뱀 머리로 해준다.
					ObjectPrint(snake[snakeLength].x, snake[snakeLength].y, w.ws_col, SNAKE);	// 커서 옮겨서 뱀 출력
					snakeLength++;												// 뱀 길이 ++

					while (snakeLength > snakeLengthMAX){	//뱀 길이가 max보다 길 때
						map[snake[0].y][snake[0].x] = EMPTY;	//뱀 꼬리 삭제
						ObjectPrint(snake[0].x, snake[0].y, w.ws_col, EMPTY);	//커서 이동 후 빈 칸 출력

						for (x = 0; x < snakeLength; x++) {		// 배열 값을 한 칸씩 이동시킨다.
							snake[x].x = snake[x + 1].x;
							snake[x].y = snake[x + 1].y;
						}

						snake[snakeLength - 1].x = snake[snakeLength - 1].y = 0;	//머리 칸 비워줌.
						snakeLength--;												//뱀 길이 --
					}

					break;
				}

				if (!IsAppleExist && snakeLength > 1) {		//맵에 사과가 없으며 && 뱀 길이가 1보다 클 때
					do {
						apple.x = (rand() % MAP_SIZE) + 1;
						apple.y = (rand() % MAP_SIZE) + 1;
					} while (map[apple.y][apple.x] != EMPTY);	//빈 칸 찾아서

					map[apple.y][apple.x] = APPLE;		//사과로 해준다.
					ObjectPrint(apple.x, apple.y, w.ws_col, APPLE);
					
					IsAppleExist = TRUE;	//"사과는 존재한다!"
				}
				
				IsSnakeMove = FALSE;	//뱀 다 움직였다.

				//시간에 따라서 난이도가 변화한다.
				timeInterval++;
				if(timeInterval % snakeGrowthInterval == 0){	//뱀이 자라나게 하는 구문.
					snakeLengthMAX++;
				}
				if(timeInterval % snakeSpeedInterval == 0){		// 뱀의 속도를 증가시켜준다.
					timer.it_value.tv_usec *= SpeedRatio;	//처음 1회에 대한 interval
					timer.it_interval.tv_usec *= SpeedRatio;	//그 이후 반복에 대한 interval
					setitimer (ITIMER_REAL, &timer, NULL);	//위에서 정의한 시간마다 SIGALRM을 발생시킨다.
					timeInterval = 0;
				}
			}
		}
	}
}

int ObjectPrint(int x, int y, int col, int ch){
	switch (ch) {
	case EMPTY:
		printf("\033[%dd\033[%dG  ", y + 1, col - MAP_SIZE*2 + 2*x - 3);
		fflush(stdout);
		break;
	case APPLE:
		printf("\033[%dd\033[%dG★", y + 1, col - MAP_SIZE*2 + 2*x - 3);
		fflush(stdout);
		break;
	case WALL:
		printf("\033[%dd\033[%dG▨", y + 1, col - MAP_SIZE*2 + 2*x - 3);
		fflush(stdout);
		break;
	case SNAKE:
		printf("\033[%dd\033[%dG■", y + 1, col - MAP_SIZE*2 + 2*x - 3);
		fflush(stdout);
		break;
	}

}

void timer_handler(int signum){
	IsSnakeMove = TRUE;
}

// void object_print(int ch) {
// 	switch (ch) {
// 	case EMPTY:
// 		printf("  ");
// 		break;
// 	case APPLE:
// 		printf("★");
// 		break;
// 	case WALL:
// 		printf("▨");
// 		break;
// 	case SNAKE:
// 		printf("■");
// 		break;
// 	}
// 	fflush(stdout);
// }
