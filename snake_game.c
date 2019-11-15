#include "main.h"
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

//정의; 맵의 크기 (토의 후 변경 가능)
#define MAP_SIZE 18

//정의; 블럭의 종류. 편의 상 정의.
#define SNAKE 0x00A0
#define APPLE 0x00A1
#define WALL 0x00A2

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

void timer_handler(int signum){
	static int count = 0;
	fprintf(stderr, "test : %d\n", count++);
}

void snakeGame(int fd[])
{
	int map[MAP_SIZE + 2][MAP_SIZE + 2] = { 0 };	//맵을 표현하기 위한 2차원 배열. 벽 때문에 +2
	int x;
	for (x = 0; x < MAP_SIZE - 1; x++) {
		map[0][x] = map[x][MAP_SIZE - 1] = map[MAP_SIZE - 1][MAP_SIZE - 1 - x] = map[MAP_SIZE - 1 - x][0] = WALL;	//맵에 벽을 만들어준다.
	}

	Pst snake[MAP_SIZE * MAP_SIZE] = { {0,0}, };	//뱀의 머리와 각 몸통의 위치를 저장할 배열.
	Pst apple = { 0,0 };	//사과의 위치를 저장할 변수.

	//Bool Is_Game = TRUE, Is_Ingame = FALSE;
	int snakeSpeed = 250; //뱀이 움직일 interval.

	char ch;	//입력 키를 저장하기 위한 변수.
	struct sigaction sa;
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM, &sa, NULL);

	struct itimerval timer;
	//처음 1회에 대한 interval
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 250000;
	//한 번 실행 이후에 대한 interval
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 250000;

	/* Start a virtual timer. It counts down whenever this process is executing. */
	setitimer (ITIMER_REAL, &timer, NULL);
	printf("timer setting complete.\n");
	
	while (TRUE)
	{
		read(fd[0], &ch, 1);	//read info to parent process

		switch (ch)
		{
		case 65:
			printf("Up!\n");
			break;

		case 66:
			printf("Down\n");
			break;

		case 67:
			printf("Right!\n");
			break;

		case 68:
			printf("Left!\n");
			break;
		}
	}
}

/*
//������ ������ũ ���� ����ǵ� ���������� �ϴ� �ٿ�����, ������, 191109.1947
//����鼭 ���� �������� ������, ������, 191109.1956

//�Լ� ����
void gotoxy(int x, int y);
void CursorView(int show);
void Snake_clear(int n);
void Map_print();
void Debug_Show();
void Object_move(int x, int y, int a);
void Object_print(int a);
void Snake_print(int n);

//���α׷� ��ü�� �����°��� ���� ����, ����� ��� ���ο� ���� ����, ��Ŭ�� ���ο� ���� ����
BOOL Is_Game = TRUE, Is_Ingame = FALSE, Is_Mode_Debug = FALSE, Is_Mode_Noclip = FALSE;

int main() {
	int Difficulty_speed[7] = 250,
	char Difficulty_Script[7][12] = { "�׽�Ʈ ���", "���� ����  ",

	int Start_time, Timer, Time_O;															//�ΰ��� ���� �ð�, ���ݱ��� �帥 �ð�, ���� �ð�
	int Snake_length, MAX_Snake_length, Snake_direction, Snake_direction_O, Snake_speed;	//�� ����, �� �ִ� ����, �� ����, �� ���� ����, ���� �ӵ�
	int Apple_x, Apple_y;																	//��� ��ǥ
	BOOL Is_Apple_exist;																	//����� ���� ����
	BOOL Is_Game_move, Is_Pause_stop;														//������ ���� ����, ������ �Ͻ����� ����, �Ͻ����� �����

	int key; //Ű���� �Է� ����

	while (Is_Game) { //������ ��� �Ѵٰ� �ϸ�

		CursorView(0);					//Ŀ�� ������
		srand((int)time(NULL));			//rand�Լ� �ʱ�ȭ

		Start_time = Timer = Time_O = Snake_length = Snake_speed = 0;
		Difficulty_num = 1;
		MAX_Snake_length = 3;
		Is_Apple_exist = Is_Pause_stop = FALSE;				//�� �� �ʱ�ȭ
		Snake_direction = East;							//�� �Ӹ��� �������� ������

		system("cls");					//ȭ�� ����
		Map_print();					//�ٽ� �־���


		for (x = 1; x <= 5; x++) {
			gotoxy(10, 4 + 2 * x);
			printf("%s\t(%4dms )", Difficulty_Script[x], Difficulty_speed[x]);
		}

		if (Difficulty_Secret == 11) {												//��ũ�� ������ �����ߴٸ�
			gotoxy(10, 16);															//���� ����� ������ ����
			Text_color(0x000C);														//���������� ĥ����
			printf("%s\t(%4dms )", Difficulty_Script[6], Difficulty_speed[6]);		//�������Դϴ� ������
			Text_color(0x0007);														//�ٽ� �Ͼ������ �ٲ���
		}

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

			if (_kbhit()) {						//Ű���� �Է��� ������
				key = _getch();						//Ű�� �ް�

				switch (key) {						//�ݴ�������� ���°� �ƴϸ� Ű�� ���� ������ �ٲ��ش�
				case 77: if (Snake_direction_O != West && Is_Game_move) Snake_direction = East; break;
				case 75: if (Snake_direction_O != East && Is_Game_move) Snake_direction = West; break;
				case 80: if (Snake_direction_O != North && Is_Game_move) Snake_direction = South; break;
				case 72: if (Snake_direction_O != South && Is_Game_move) Snake_direction = North; break;

				case 13: Is_Game_move = FALSE; break;							//���͸� �Է¹����� ������ ��� �����
				case 110:													//n�� ������
					if (Is_Mode_Debug) Is_Mode_Noclip = 1 - Is_Mode_Noclip;	//����� ����� ��Ŭ�� ��带 ���� ����
					Debug_Show();											//��Ŭ�� ����� üũ����
					break;
				}

				if (Is_Game_move == FALSE) {		//������ �Ͻ������Ǹ� ����ó�� �������
					gotoxy(54, 9);
					printf("P A U S E");
					gotoxy(45, 11);
					printf("Continue\t\tGo To Menu");
					gotoxy(45, 12);
					printf(" [Enter] \t\t  [Space]");

					if (_kbhit) {			//Ű���� �Է��� �ް�
						key = _getch();

						switch (key) {
						case 13:						//���͸� ������
							Is_Game_move = TRUE;			//���� ��� �Ұ���
							gotoxy(54, 9);				//�Ͻ������϶� ������ ������
							printf("                     ");
							gotoxy(45, 11);
							printf("                                ");
							gotoxy(45, 12);
							printf("                            ");
							break;

						case 32:						//�����̽���
							Is_Ingame = FALSE;				//���� ���Ұ��� �Ѥ� ������ũ ����
							Is_Pause_stop = TRUE;			//�ٷ� ����
							gotoxy(54, 9);					//�Ͻ������϶� ������ ������
							printf("                     ");
							gotoxy(45, 11);
							printf("                                ");
							gotoxy(45, 12);
							printf("                            ");
							break;
						}
					}
				}
			}

			gotoxy(23, 21);														//������ ����
			printf("���� :    %4d ��\n", Snake_length*Difficulty_num);			//���� ����Ʈ����

			if (Is_Mode_Debug) {												//����� �� �� �׽�Ʈ �ڵ���
				printf("MAX_Snake_length = %d\n", MAX_Snake_length);				//�� �ִ����
				printf("Speed = %d (ms)\n", Difficulty_speed[Difficulty_num]);		//�ӵ�
				printf("Snake_direction = %d\n", Snake_direction);					//�� �Ӹ� ����
				printf("timer = %d", Timer);										//�����ð�

				for (x = 0; x < 20; x++) {
					for (y = 0; y < 20; y++) {
						gotoxy(45 + 2 * x, y);										//���� ȭ�� ����
						printf("%d ", Map[y][x]);									//�׸��� �ƴ϶� ���� �� ��ü�� ���
					}
				}
			}
		}

		//Is_Ingame�� 0�̸� (== ������ ��������)
		fflush(stdin);	//���ݱ��� �Է¹��� ���� �� �����

		if (Is_Pause_stop) {			//�Ͻ������ؼ� ���� ���̶��
			Snake_clear(Snake_length);		//���� �ʱ�ȭ
			Map[Apple_y][Apple_x] = 0;		//�ʵ� �ʱ�ȭ
			Apple_x = Apple_y = 0;
		}

		else{							//���������� ����� ���̸�
			gotoxy(45, 5);																		//�����ٰ�
			char Gameover[12] = { 'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R','.','.','.' };	//���� ���� �۾�
			for (x = 0; x < 12; x++) {
				printf("%c ", Gameover[x]);														//gameover �۾��� ���ʴ�� �����
				Sleep(100);																		//��� ����
			}

			//���� ���� ȭ���� ������ش�
			gotoxy(45, 9);
			printf("�ٽ� �÷��� �Ͻðڽ��ϱ�?");
			gotoxy(45, 11);
			printf("  Yes\t\t  No");
			gotoxy(45, 12);
			printf("[Enter]\t\t[Space]");

			if ((Difficulty_num < 6) && (Snake_length*Difficulty_num  >= 100)) { //������ �̸� ���̵����� 100�� �̻��
				gotoxy(45, 10);
				Text_color(0x0004);							//����
				printf("���̵� ���� â + �顿10 = ...?");	//������ �ر� ��Ʈ�� �˷��ش�
				Text_color(0x0007);							//�Ͼ�
			}

			BOOL Is_Game_question = TRUE;	//�� ȭ���� ������ų ����
			while (Is_Game_question) {		// Ű ������ ������ ���� ����,
				key = _getch();

				switch (key) {
				case 32:						//�����̽� ������
					gotoxy(0, 23);					//���� ȭ�� ������ ���� (�� ������ ���� ������ �ؿ� �߰� �ϱ� ����)
					Is_Game = FALSE;					//�� ���� ����
					Is_Game_question = FALSE;			//�� ȭ�鵵 ����
					break;

				case 13:						//���� ������
					Snake_clear(Snake_length);		//���� �ʱ�ȭ
					Map[Apple_y][Apple_x] = 0;		//�ʵ� �ʱ�ȭ
					Apple_x = Apple_y = 0;			//��� ��ǥ�� �ʱ�ȭ
					Is_Game_question = FALSE;		//�� ȭ�鵵 ����
					break;
				}
			}
		}
	}
}

//��ǥ ���� �Լ�
void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

//Ŀ�� �ȱ����̰� �ϴ� �Լ�
void CursorView(int show) {
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
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

void Debug_Show() {
	if (Is_Mode_Debug) {	//����� ����̸�
		gotoxy(0, 0);			//ȭ�� �»�ܿ�
		Text_color(0x0003);		//�Ķ�
		printf("��");			//�����!
		Text_color(0x0007);		//�Ͼ�
	}
	else {
		gotoxy(0, 0);			//ȭ�� �»�ܿ�
		printf("��");			//�� �Ϲ� �÷��̾�~
	}

	if (!(!Is_Mode_Debug || Is_Ingame)) {	//����� ����̸鼭 �޴��϶�
		gotoxy(45, 3);						//������ ���
		printf("****�����  ���****");
		gotoxy(45, 5);
		printf("[d] ����� ��� ����");
		gotoxy(45, 6);
		printf("[n] ��Ŭ�� ��� ����");
		gotoxy(45, 7);
		printf("[s] �ӵ� ���Ƿ� �Է�");
	}

	else {									//�ƴϸ�
		gotoxy(45, 3);						//�� ����
		printf("                    ");
		gotoxy(45, 5);
		printf("                    ");
		gotoxy(45, 6);
		printf("                    ");
		gotoxy(45, 7);
		printf("                    ");
	}

	if (Is_Mode_Noclip) {
		gotoxy(38, 0);
		Text_color(0x0006);
		printf("��");
		Text_color(0x0007);
	}
	else {
		gotoxy(38, 0);
		printf("��");
	}

	if (!(!Is_Mode_Noclip || Is_Ingame)) {
		gotoxy(61, 6);
		printf("����");
	}
	else if (!(!Is_Mode_Debug || Is_Ingame)) {
			gotoxy(61, 6);
			printf("����");
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
