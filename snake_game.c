#include "main.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
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

//타이머가 실행될 때마다 실행되는 함수. 여기서 뱀을 움직일 것.
void snake_move(int signum){
	static int count = 0;
	fprintf(stderr, "test : %d\n", count++);
}

void snakeGame(int fd[])
{	
	//다음의 구문은 단위 시간마다 Signal을 발생시켜, 뱀을 규칙적으로 움직이게 하기 위한 구조체들을 정의하기 위한 구문이다.
	///SIGALRM 처리를 위한 구조체.
	struct sigaction sa;
	sa.sa_handler = &snake_move;
	sigaction(SIGALRM, &sa, NULL);
	
	///SIGALAM의 interval을 정의하기 위한 구조체.
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 250000;	//처음 1회에 대한 interval
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 250000;	//한 번 실행 이후에 대한 interval
	setitimer (ITIMER_REAL, &timer, NULL);	//위에서 정의한 시간마다 SIGALRM을 발생시킨다.

	//다음의 구문은 게임에서 사용할 변수들을 정의하기 위한 구문이다.
	///0. 맵
	int map[MAP_SIZE + 2][MAP_SIZE + 2] = { EMPTY, };	//맵을 표현하기 위한 2차원 배열. 벽 때문에 +2
	int x;
	for (x = 0; x < MAP_SIZE - 1; x++) {
		map[0][x] = map[x][MAP_SIZE - 1] = map[MAP_SIZE - 1][MAP_SIZE - 1 - x] = map[MAP_SIZE - 1 - x][0] = WALL;	//맵에 벽을 만들어준다.
	}

	///1. 뱀
	Pst snake[MAP_SIZE * MAP_SIZE] = { {0,0}, };	//뱀의 머리와 각 몸통의 위치를 저장할 배열.
	int snakeDirection = EAST;
	int snakeDirection_OLD = EAST;

	///2. 사과
	Pst apple = { 0,0 };	//사과의 위치를 저장할 변수.
	
	char ch;	//입력 키를 저장하기 위한 변수.

	while (TRUE)
	{
		int readByte;
		ch = 0;
		readByte = read(fd[0], &ch, 1);	//부모 프로세스에서 pipe를 통해 키보드 값을 읽어옴

		if(readByte == 1){	//값을 읽어온 경우에만
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
	}
}

/*

int main() {
	int Snake_length, MAX_Snake_length, Snake_direction, Snake_direction_O, Snake_speed;	//�� ����, �� �ִ� ����, �� ����, �� ���� ����, ���� �ӵ�
	int Apple_x, Apple_y;																	//��� ��ǥ
	BOOL Is_Apple_exist;																	//����� ���� ����
	BOOL Is_Game_move, Is_Pause_stop;														//������ ���� ����, ������ �Ͻ����� ����, �Ͻ����� �����

	while (Is_Game) { //������ ��� �Ѵٰ� �ϸ�

		srand((int)time(NULL));			//rand�Լ� �ʱ�ȭ

		Start_time = Timer = Time_O = Snake_length = Snake_speed = 0;
		MAX_Snake_length = 3;
		Is_Apple_exist = Is_Pause_stop = FALSE;				//�� �� �ʱ�ȭ
		Snake_direction = East;							//�� �Ӹ��� �������� ������

		system("cls");					//ȭ�� ����
		Map_print();					//�ٽ� �־���

		Is_Ingame = Is_Game_move = TRUE;	//���� �����Ѵ�!!!!

		system("cls");					//���̵� â ����
		Map_print();					//�ٽ� �̻ڰ� ����Ʈ

		Start_time = clock();			//���۽ð�
		Snake_x[0] = Snake_y[0] = 9;	//���� ��ġ
		gotoxy(18, 9);
		Text_color(0x002F);				//�� �Ӹ� ���
		Snake_print(0);
		Text_color(0x0007);
		Snake_length++;					//�Ӹ� �������� ���� 1��

		while (Is_Ingame) {
			Timer = (int)((clock() - Start_time) / Snake_speed); //���� �ð��� ���

			//여기부터 단위시간마다 뱀이 움직이는 부분인거 같음
			if (Timer != Time_O && Is_Game_move) { //�ð��� ������

				switch (Snake_direction) {
				case East: //���� �������� ��ǥ�� ������ (���۽� �⺻���� ����������)
					Snake_x[Snake_length] = Snake_x[Snake_length - 1] + 1;
					Snake_y[Snake_length] = Snake_y[Snake_length - 1];
					Snake_direction_O = East;
					break;
				case West: //��
					Snake_x[Snake_length] = Snake_x[Snake_length - 1] - 1;
					Snake_y[Snake_length] = Snake_y[Snake_length - 1];
					Snake_direction_O = West;
					break;
				case South: //��
					Snake_x[Snake_length] = Snake_x[Snake_length - 1];
					Snake_y[Snake_length] = Snake_y[Snake_length - 1] + 1;
					Snake_direction_O = South;
					break;
				case North: //��
					Snake_x[Snake_length] = Snake_x[Snake_length - 1];
					Snake_y[Snake_length] = Snake_y[Snake_length - 1] - 1;
					Snake_direction_O = North;
					break;
				}

				switch (Map[Snake_y[Snake_length]][Snake_x[Snake_length]]) { // �Ӹ��� �Ű��� ĭ��
				case Wall:										//���̸�
					if (!Is_Mode_Debug) Is_Ingame = FALSE;				//����� ��尡 �ƴ� �� ���� ��
					break;

				case Apple:										//�����
					MAX_Snake_length++;								//�Ѱ� ���̸� �÷��ְ�
					if (MAX_Snake_length == 324) Is_Ingame = FALSE;
					Is_Apple_exist = 0;								//����� ���ٰ� �˷��ش�
																	//break�� ���� ������ ��� �԰� �ؿ� �Լ� �����϶��

				case Snake:										//���̸�
					if (Is_Apple_exist) {							//����� �����ϸ� == case Apple�� ������ �ʾ�����
						if (!Is_Mode_Debug) {						//����� ��尡 �ƴѵ�
							if (!((Snake_x[0] == Snake_x[Snake_length]) && (Snake_y[0] == Snake_y[Snake_length]))) Is_Ingame = FALSE; break;	//������ �ƴϸ� ���� ��
						}
						else if (!Is_Mode_Noclip) break;			//��Ŭ�� ���� �� ���� �������
					}

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

			//여기부터 키보드 입력에 관한 부분인 듯.

			if (_kbhit()) {						//Ű���� �Է��� ������
				key = _getch();						//Ű�� �ް�

				switch (key) {						//�ݴ�������� ���°� �ƴϸ� Ű�� ���� ������ �ٲ��ش�
				case 77: if (Snake_direction_O != West && Is_Game_move) Snake_direction = East; break;
				case 75: if (Snake_direction_O != East && Is_Game_move) Snake_direction = West; break;
				case 80: if (Snake_direction_O != North && Is_Game_move) Snake_direction = South; break;
				case 72: if (Snake_direction_O != South && Is_Game_move) Snake_direction = North; break;
				}
			}
		}
	}
}

//�� �ʱ�ȭ �ϴ� �Լ�
void Snake_clear(int n) {
	for (x = 0; x < n; x++) {
		Map[Snake_y[x]][Snake_x[x]] = 0;	//���� �ʱ�ȭ��
		Snake_x[x] = Snake_y[x] = 0;		//�� �迭�� �ʱ�ȭ��
	}
	Snake_x[n] = Snake_y[n] = 0;			//�� ������ �쵵 �ʱ�ȭ����
}

//�� ����Ʈ ���ִ� �Լ�
void Map_print() {
	for (x = 0; x < 20; x++) {
		for (y = 0; y < 20; y++) {
			//Text_color((x+y)%16)
			gotoxy(2 * x, y);
			Object_print(Map[y][x]);
		}
	}
	//Text_color(0x0007);
	Debug_Show();
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

//Ư�� ������� ���� ������ִ� �Լ�
void Snake_print(int n) {
	int Dx_Before, Dy_Before, Dx_After, Dy_After;	//�� ������ �������� ��/�� ������ ��ǥ ��ȭ
	BOOL Up, Down, Left, Right;						//�� ������ �������� �����¿쿡 ������ �ִ��� üũ�ϴ� ����
	Up = Down = Left = Right = FALSE;				//�ϴ� �� ������ �����¿쿡 ���� �����ϴ�
	Map[Snake_y[n]][Snake_x[n]] = Snake;			//�׸��� �ϴ� �� ��ǥ���� ���� �ֽ��ϴ�

	if (n != 0) {									//�̰� ������ �ƴ϶��
		Dx_Before = Snake_x[n] - Snake_x[n - 1];		// Dx = 1; ���� ĭ�� ����, Dx = -1; ���� ĭ�� ������
		Dy_Before = Snake_y[n] - Snake_y[n - 1];		// Dy = 1; ���� ĭ�� �Ʒ���, Dy = -1; ���� ĭ�� ����
	}
	else Dx_Before = Dy_Before = 0;					//������ ���� ���� ���ٰ� �˷���

	if (Snake_x[n + 1] != 0) {						//�̰� �Ӹ��� �ƴ϶�� (�Ӹ� ���� ���İ��� ��ǥ���� 0���� ���������Ƿ�)
		Dx_After = Snake_x[n] - Snake_x[n + 1];			// Dx = 1; ���� ĭ�� ����, Dx = -1; ���� ĭ�� ������
		Dy_After = Snake_y[n] - Snake_y[n + 1];			// Dy = 1; ���� ĭ�� �Ʒ���, Dy = -1; ���� ĭ�� ����
	}
	else Dx_After = Dy_After = 0;					//�Ӹ��� ���� ���� ���ٰ� �˷���

	if (Dx_Before == 1 || Dx_After == 1)	Left = TRUE;	//���ʿ� ���� �ֳ�??
	if (Dx_Before == -1 || Dx_After == -1)	Right = TRUE;	//������
	if (Dy_Before == 1 || Dy_After == 1)	Up = TRUE;		//����
	if (Dy_Before == -1 || Dy_After == -1)	Down = TRUE;	//�Ʒ���

	gotoxy(2 * Snake_x[n], Snake_y[n]);				//�� ĭ���� ����

	//���� �������� ���ڴ� Ű�е� ������ (ex (24) = ��, �¿� ���� ����, (20) == �󿡸� ���� ����)
	if (Up) {							//���� ���� ����
		if (Left) puts("��");				//���ʿ� ���� ����						(24)
		else if (Right) puts("��");			//�����ʿ� ���� ����					(26)
		else puts("��");					//�Ʒ��� ���� �ִ� ���� �������		(20, 28)
	}

	else if (Left) {					//���ʿ� ���� ����
		if (Down) puts("��");				//�Ʒ��ʿ� ���� ����					(48)
		else puts("��");					//�����ʿ� ���� �ִ� ���� �������		(40, 46)
	}

	else if (Right) {					//�����ʿ� ���� ����
		if (Down) puts("��");				//�Ʒ��ʿ� ���� ����					(68)
		else printf("��");					//�̹� ���¿� ���� �����Ƿ�				(60)
	}

	else if(Down) printf("��");			//�̹� �����¿� ���� �����Ƿ�				(80)

	else printf("��");					//�� ó�� ������ ������ ���ϹǷ�			(00)
}
*/
