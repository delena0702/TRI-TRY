#include "main.h"
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
void object_print(int ch);

void snakeGame(int fd[])
{	
	//다음의 구문은 단위 시간마다 Signal을 발생시켜, 뱀을 규칙적으로 움직이게 하기 위한 구조체들을 정의하기 위한 구문이다.
	///SIGALRM 처리를 위한 구조체.
	struct sigaction sa;
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM, &sa, NULL);
	
	///SIGALAM의 interval을 정의하기 위한 구조체.
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 250000;	//처음 1회에 대한 interval
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 250000;	//한 번 실행 이후에 대한 interval
	setitimer (ITIMER_REAL, &timer, NULL);	//위에서 정의한 시간마다 SIGALRM을 발생시킨다.

	//다음의 구문은 스네이크 게임을 다른 게임과 함께 출력하기 위해 위치를 조정하고자, 창 크기를 받아오는 구문이다.
	//터미널의 크기를 반각 글자 단위로 받아온다.
	struct winsize w;
   	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	///화면 크기를 출력하는 테스트용 코드.
   	printf ("lines %d\n", w.ws_row);
    printf ("columns %d\n", w.ws_col);

	//다음의 구문은 게임에서 사용할 변수들을 정의하기 위한 구문이다.
	int x, y;

	///0. 맵
	int map[MAP_SIZE + 2][MAP_SIZE + 2];	//맵을 표현하기 위한 2차원 배열. 벽 때문에 +2
	for(y = 0; y < MAP_SIZE; y++){
		for(x = 0; x < MAP_SIZE; x++){
			map[y][x] = EMPTY;
		}
	}
	for (x = 0; x < MAP_SIZE - 1; x++) {
		map[0][x] = map[x][MAP_SIZE - 1] = map[MAP_SIZE - 1][MAP_SIZE - 1 - x] = map[MAP_SIZE - 1 - x][0] = WALL;	//맵에 벽을 만들어준다.
	}

	///1. 뱀
	Pst snake[MAP_SIZE * MAP_SIZE] = { {0,0}, };	//뱀의 머리와 각 몸통의 위치를 저장할 배열.
	int snakeLength = 0;
	int snakeLengthMAX = 3;	//처음에는 3칸으로 시작한다.
	int snakeDirection = EAST;
	int snakeDirection_OLD = EAST;

	///2. 사과
	Pst apple = { 0,0 };	//사과의 위치를 저장할 변수.
	Bool IsAppleExist;	//사과 존재 여부.
	
	char ch;	//입력 키를 저장하기 위한 변수.

	//다음 구문은 게임을 초기화한다.
	srand((int)time(NULL));

	IsSnakeMove = FALSE;
	snake[0].x = snake[0].y = MAP_SIZE / 2;
	map[snake[0].y][snake[0].x] = SNAKE;
	snakeLength++;

	IsAppleExist = TRUE;
	apple.x = (rand() % MAP_SIZE) + 1;
	apple.y = (rand() % MAP_SIZE) + 1;
	map[apple.y][apple.x] = APPLE;
	
	//맵 출력
	for(y = 0; y < MAP_SIZE; y++){
		for(x = 0; x < MAP_SIZE; x++){
			gotoxy(w.ws_col - 2 - MAP_SIZE*2 + 2*x, y + 1);
			//fprintf(stderr, "%d ", map[y][x]);
			object_print(map[y][x]);
		}
		fprintf(stderr, "\n");
	}

	while (TRUE) {
		int readByte;
		ch = 0;
		readByte = read(fd[0], &ch, 1);	//부모 프로세스에서 pipe를 통해 키보드 값을 읽어옴

		if(readByte == 1) {	//값을 읽어온 경우에만
			switch (ch)
			{
			case 65:	//위
				printf("Up!\n");
				if(snakeDirection_OLD != SOUTH) snakeDirection = NORTH;
				break;

			case 66:	//아래
				printf("Down\n");
				if(snakeDirection_OLD != NORTH) snakeDirection = SOUTH;
				break;

			case 67:	//오른쪽
				printf("Right!\n");
				if(snakeDirection_OLD != WEST) snakeDirection = EAST;
				break;

			case 68:	//왼쪽
				printf("Left!\n");
				if(snakeDirection_OLD != EAST) snakeDirection = WEST;
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
					if (!((snake[0].x == snake[snakeLength].x) && (snake[0].y == snake[snakeLength].y)))
						return;	//머리 위치와 꼬리 위치가 아니라면 게임 끝. 꼬리는 바로 움직여서 꼬리 위치는 빈 칸이 될 것이므로 종료시키지 않아도 된다.
					//break를 씌우지 않고 넘김으로써 EMPTY와 같이 처리되도록 한다.

				case EMPTY:
					snakeLength++;
					if(snakeLength > snakeLengthMAX){

					}
					break;
				}
				
				IsSnakeMove = FALSE;
			}
		}
	}
}

void timer_handler(int signum){
	IsSnakeMove = TRUE;
}

void object_print(int ch) {
	switch (ch) {
	case EMPTY:
		fprintf(stderr, "☆");
		break;
	case APPLE:
		fprintf(stderr, "★");
		break;
	case WALL:
		fprintf(stderr, "▨");
		break;
	case SNAKE:
		fprintf(stderr, "■");
		break;
	}
}

/*

int main() {

	while (Is_Game) { //������ ��� �Ѵٰ� �ϸ�

		while (Is_Ingame) {
			Timer = (int)((clock() - Start_time) / Snake_speed); //���� �ð��� ���

			//여기부터 단위시간마다 뱀이 움직이는 부분인거 같음
			if (Timer != Time_O && Is_Game_move) { //�ð��� ������

				switch (Map[Snake_y[Snake_length]][Snake_x[Snake_length]]) { // �Ӹ��� �Ű��� ĭ��
				case 0:											//�� �����̸�
					Snake_length++;									//�� ����++
					if (Snake_length > MAX_Snake_length) {			//�� ���̰� �ִ븦 ������
						Object_move(Snake_x[0], Snake_y[0], 0);			//�ϴ� ������ ĭ�� ������

						for (x = 0; x < Snake_length; x++) {			//�� ĭ�� ��ǥ�� �Ű��ش�
							Snake_x[x] = Snake_x[x + 1];
							Snake_y[x] = Snake_y[x + 1];
						}

						Snake_x[MAX_Snake_length] = Snake_y[MAX_Snake_length] = 0; //�׷� ������(m��°�� �ϸ�)�̶� m-1�̶� ��ġ�ϱ� �������� ������
						Snake_length--;								 //�ִ� ���̷� �����ش�
					}

					Text_color(0x0027);										//������ �ʷϹ�濡 �����
					for (x = 0; x < Snake_length - 1; x++) Snake_print(x);	//�� ��翡 �°� �ٽ� ����Ʈ ���ش�
					Text_color(0x002F);										//���� �ʷϹ�濡 ���� ��� ��
					Snake_print(Snake_length - 1);							//�󱼵� ����Ʈ ����
					Text_color(0x0007);										//�� �ٽ� �������
					break;
				}
				Time_O = Timer; //�ð� �ʱ�ȭ
			}

			if (!Is_Apple_exist && MAX_Snake_length != 324) {		//����� �ʿ� ���� �ִ� ���̰� �ƴϸ�
				do {
					Apple_x = (rand() % 18) + 1;
					Apple_y = (rand() % 18) + 1;
				} while (Map[Apple_y][Apple_x] != 0);				//�������� ���� ��ǥ�� ��ĭ���� Ȯ�� ��

				Object_move(Apple_x, Apple_y, Apple);				//��� ����Ʈ ���� ��
				Is_Apple_exist = TRUE;								//��� �ڿ� ��� �ִٰ� ���ش�
			}
		}
	}
}

//�� ĭ�� ������Ʈ�� �ٲ��ִ� �Լ�
void Object_move(int x, int y, int a) {
	Map[y][x] = a;
	gotoxy(2 * x, y);
	Object_print(a);
}

//������Ʈ�� ������ִ� �Լ�
void Object_print(int a) {
	switch (a) {
		case 0:
			puts("  ");
			break;

		case Apple:
			Text_color(0x000C);
			puts("��");
			Text_color(0x0007);
			break;

		case Wall:
			puts("��");
			break;
	}
}
*/
