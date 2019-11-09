#include "main.h"

//이차원 배열에서 사용하기 쉽도록 각각의 블럭을 정의해준다
#define MAP_SIZE 18;
#define SNAKE 0x00A0
#define APPLE 0x00A1
#define WALL 0x00A2

//방향도 정의해줌으로써 이해하기 쉽게 만들어준다
#define EAST 0x00B0
#define WEST 0x00B1
#define SOUTH 0x00B2
#define NOUTH 0x00B3

typedef struct snakePosition {
	int x;
	int y;
} snakePst;

void snakeGame(int fd[])
{
	int map[MAP_SIZE + 2][MAP_SIZE + 2] = { 0 };	//맵을 표현할 2차원 변수, 벽을 포함해서 가로,세로 모두 +2
	snakePst snake[MAP_SIZE * MAP_SIZE] = { {0,0}, };	//뱀의 위치를 저장해줄 변수.

	for (int x = 0; x < MAP_SIZE - 1; x++) {
		Map[0][x] = Map[x][19] = Map[19][19 - x] = Map[19 - x][0] = WALL;	//맵에 벽을 만들어준다.
	}

	char ch;	//키보드 값 받는 변수.
	while (true)
	{
		read(fd[0], &ch, 1);	//부모 프로세스에서 값을 받아온다

		switch (ch)	//키보드 입력에 의해 분기
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
//예전에 스네이크 게임 만든건데 참고용으로 일단 붙여넣음, 김재현, 191109.1947
//만들면서 점점 지워나갈 예정임, 김재현, 191109.1956

//함수 정의
void gotoxy(int x, int y);
void CursorView(int show);
void Snake_clear(int n);
void Map_print();
void Debug_Show();
void Object_move(int x, int y, int a);
void Object_print(int a);
void Snake_print(int n);

//프로그램 자체를 돌리는가에 대한 변수, 디버그 모드 여부에 대한 변수, 노클립 여부에 대한 변수
BOOL Is_Game = TRUE, Is_Ingame = FALSE, Is_Mode_Debug = FALSE, Is_Mode_Noclip = FALSE;

int main() {
	int Difficulty_speed[7] = { 0, 250, 200, 150, 120, 80, 44 }, Difficulty_num, Difficulty_Secret = 0;												//난이도 속도, 번호, 시크릿
	char Difficulty_Script[7][12] = { "테스트 모드", "아주 쉬움  ", "   쉬움    ", "   보통    ", "  어려움   ", "아주 어려움", "  지옥급   "  };	//난이도 이름

	int Start_time, Timer, Time_O;															//인게임 시작 시간, 지금까지 흐른 시간, 원래 시간
	int Snake_length, MAX_Snake_length, Snake_direction, Snake_direction_O, Snake_speed;	//뱀 길이, 뱀 최대 길이, 뱀 방향, 뱀 저번 방향, 뱀의 속도
	int Apple_x, Apple_y;																	//사과 좌표
	BOOL Is_Apple_exist;																	//사과의 존재 여부
	BOOL Is_Game_move, Is_Pause_stop;														//게임의 지속 여부, 게임의 일시정지 여부, 일시정시 종료시

	int key; //키보드 입력 변수

	while (Is_Game) { //게임을 계속 한다고 하면

		CursorView(0);					//커서 가리기
		srand((int)time(NULL));			//rand함수 초기화

		Start_time = Timer = Time_O = Snake_length = Snake_speed = 0;
		Difficulty_num = 1;
		MAX_Snake_length = 3;
		Is_Apple_exist = Is_Pause_stop = FALSE;				//싹 다 초기화
		Snake_direction = East;							//뱀 머리를 동쪽으로 맞춰줌

		system("cls");					//화면 비우고
		Map_print();					//다시 넣어줌

		//난이도 선택 창
		gotoxy(4, 3);
		printf(" 원하시는 난이도를 선택해주세요.");
		gotoxy(4, 4);
		printf("  (↑↓로 고르기, Enter로 선택)");
		gotoxy(6, 6);
		printf("▶");

		for (x = 1; x <= 5; x++) {
			gotoxy(10, 4 + 2 * x);
			printf("%s\t(%4dms )", Difficulty_Script[x], Difficulty_speed[x]);
		}

		if (Difficulty_Secret == 11) {												//시크릿 조건을 충족했다면
			gotoxy(10, 16);															//아주 어려움 밑으로 가서
			Text_color(0x000C);														//빨강색으로 칠해줌
			printf("%s\t(%4dms )", Difficulty_Script[6], Difficulty_speed[6]);		//지옥급입니다 여러분
			Text_color(0x0007);														//다시 하얀색으로 바꿔줌
		}

		while (Snake_speed == 0) { //난이도(속도)가 정해지기 전까지
			if (_kbhit()) {
				key = _getch();

				switch (key) {
				case 72:													//위 버튼 누르면
					if (Difficulty_num > 1) {								//가장 위가 아니라면
						gotoxy(6, 2 * (Difficulty_num + 2));					//원래 있던 자리를
						printf("  ");											//비우고
						gotoxy(6, 2 * (Difficulty_num + 1));					//위에
						printf("▶");											//삼각형 넣어줌
						Difficulty_num--;										//난이도 하락
					}
					break;

				case 80:													//아래 버튼 누르면
					if (Difficulty_num < 5 + (Difficulty_Secret / 10)) {	//가장 아래(시크릿 고려)가 아니라면
						gotoxy(6, 2 * (Difficulty_num + 2));					//원래 있던 자리를
						printf("  ");											//비우고
						gotoxy(6, 2 * (Difficulty_num + 3));					//아래에
						printf("▶");											//삼각형 넣어줌
						Difficulty_num++;										//난이도 상승
					}

					else if ((Difficulty_num == 5) && (Difficulty_Secret <= 10)) {	//만약 시크릿이 안열렸다면
						Difficulty_Secret++;											//시크릿 변수++
						if (Difficulty_Secret == 10) {									//시크릿 조건 달성시
							gotoxy(10, 16);												//아주 어려움 밑으로 가서
							Text_color(0x000C);											//빨강색으로 칠해줌
							printf("%s\t(%4dms )", Difficulty_Script[6], Difficulty_speed[6]);
							Text_color(0x0007);											//다시 하얀색
							Difficulty_Secret++;										//++해줌으로써 다시는 이 함수가 실행되지 못하게 한다
						}
					}
					break;

				case 100:													//d를 누르면
					Is_Mode_Debug = 1 - Is_Mode_Debug;						//디버그 모드를 조정 가능
					if (!Is_Mode_Debug) Is_Mode_Noclip = FALSE;					//디버그 모드가 아니면 노클립 모드도 해제해준다
					Debug_Show();											//디버그인지 아닌지 표시해준다
					break;

				case 110:													//n을 누르면
					if (Is_Mode_Debug) Is_Mode_Noclip = 1 - Is_Mode_Noclip;	//디버그 모드라면 노클립 모드를 조정 가능
					Debug_Show();											//노클립 모든지 체크해줌
					break;

				case 115:													//s를 누르면
					gotoxy(6, 2 * (Difficulty_num + 2));				//삼각형을 아예 지워버림
					printf("  ");
					gotoxy(6, 18);
					printf("▶");
					gotoxy(10, 18);										//테스트 모드 진입 창을 출력
					printf("%s\t(    ms )", Difficulty_Script[0]);

					do {
						gotoxy(25, 18);
						printf("    ");
						gotoxy(25, 18);
						scanf_s("%d", &Difficulty_speed[0]);
					} while (Difficulty_speed[0] < 0);		//0보다 클 때까지

					Difficulty_num = 0;						//난이도를 테스트 모드로 확정
					Snake_speed = Difficulty_speed[0];		//입력한 값으로 속도 확정

					if (Difficulty_speed[0] == 0){			//만약 속도를 0으로 입력했다면
						Difficulty_num = 1;					//처음(아주 쉬움)으로 다시 설정해줌
					gotoxy(6, 6);
					printf("▶");
					gotoxy(6, 18);							//속도 선택창을 지움
					printf("                           ");
					}
					break;

				case 13: Snake_speed = Difficulty_speed[Difficulty_num]; break; //엔터치면 속도를 정한다 = while문 탈출

				case 32:								//스페이스를 누르면
					gotoxy(0, 23);						//밑으로 가서
					return 0;							//게임 종료
				}
			}
		}

		Is_Ingame = Is_Game_move = TRUE;	//게임 시작한다!!!!

		system("cls");					//난이도 창 제거
		Map_print();					//다시 이쁘게 프린트

		gotoxy(17, 6);
		printf("READY?");
		Sleep(2000);					//대기 까 플레이어야
		gotoxy(17, 6);
		printf("      ");				//레디 글씨 지워줌

		gotoxy(0, 21);
		printf("난이도 : ");								//난이도는 한번만 띄워주면 되므로 while문 밖에 써준다
		if (Difficulty_num == 6) Text_color(0x000C);			//지옥급만 빨강색으로 칠해줌
		printf("%s\n", Difficulty_Script[Difficulty_num]);		//난이도명 출력
		if (Difficulty_num == 6) Text_color(0x0007);			//지옥급만 하얀색으로 다시 칠해줌
		gotoxy(45, 1);
		printf("[Enter] PAUSE");

		Start_time = clock();			//시작시간
		Snake_x[0] = Snake_y[0] = 9;	//시작 위치
		gotoxy(18, 9);
		Text_color(0x002F);				//뱀 머리 출력
		Snake_print(0);
		Text_color(0x0007);
		Snake_length++;					//머리 나왔으니 길이 1됨

		while (Is_Ingame) {
			Timer = (int)((clock() - Start_time) / Snake_speed); //단위 시간을 잰다

			if (Timer != Time_O && Is_Game_move) { //시간이 지나면

				switch (Snake_direction) {
				case East: //동의 방향으로 좌표를 움직임 (시작시 기본으로 맞춰져있음)
					Snake_x[Snake_length] = Snake_x[Snake_length - 1] + 1;
					Snake_y[Snake_length] = Snake_y[Snake_length - 1];
					Snake_direction_O = East;
					break;
				case West: //서
					Snake_x[Snake_length] = Snake_x[Snake_length - 1] - 1;
					Snake_y[Snake_length] = Snake_y[Snake_length - 1];
					Snake_direction_O = West;
					break;
				case South: //남
					Snake_x[Snake_length] = Snake_x[Snake_length - 1];
					Snake_y[Snake_length] = Snake_y[Snake_length - 1] + 1;
					Snake_direction_O = South;
					break;
				case North: //북
					Snake_x[Snake_length] = Snake_x[Snake_length - 1];
					Snake_y[Snake_length] = Snake_y[Snake_length - 1] - 1;
					Snake_direction_O = North;
					break;
				}

				switch (Map[Snake_y[Snake_length]][Snake_x[Snake_length]]) { // 머리가 옮겨질 칸이
				case Wall:										//벽이면
					if (!Is_Mode_Debug) Is_Ingame = FALSE;				//디버그 모드가 아닐 시 게임 끗
					break;

				case Apple:										//사과면
					MAX_Snake_length++;								//한계 길이를 늘려주고
					if (MAX_Snake_length == 324) Is_Ingame = FALSE;
					Is_Apple_exist = 0;								//사과가 없다고 알려준다
																	//break가 없는 이유는 사과 먹고 밑에 함수 실행하라고

				case Snake:										//뱀이면
					if (Is_Apple_exist) {							//사과가 존재하면 == case Apple을 지나지 않았으면
						if (!Is_Mode_Debug) {						//디버그 모드가 아닌데
							if (!((Snake_x[0] == Snake_x[Snake_length]) && (Snake_y[0] == Snake_y[Snake_length]))) Is_Ingame = FALSE; break;	//꼬리도 아니면 게임 끝
						}
						else if (!Is_Mode_Noclip) break;			//노클립 모드면 빈 공간 취급해줌
					}

				case 0:											//빈 공간이면
					Snake_length++;									//몸 길이++
					if (Snake_length > MAX_Snake_length) {			//몸 길이가 최대를 넘으면
						Object_move(Snake_x[0], Snake_y[0], 0);			//일단 마지막 칸을 지워줌

						for (x = 0; x < Snake_length; x++) {			//한 칸씩 좌표를 옮겨준다
							Snake_x[x] = Snake_x[x + 1];
							Snake_y[x] = Snake_y[x + 1];
						}

						Snake_x[MAX_Snake_length] = Snake_y[MAX_Snake_length] = 0; //그럼 마지막(m번째라 하면)이랑 m-1이랑 겹치니까 마지막을 지워줌
						Snake_length--;								 //최대 길이로 돌려준다
					}
					Text_color(0x0027);										//몸통은 초록배경에 흰색선
					for (x = 0; x < Snake_length - 1; x++) Snake_print(x);	//뱀 모양에 맞게 다시 프린트 해준다
					Text_color(0x002F);										//얼굴은 초록배경에 밝은 흰색 선
					Snake_print(Snake_length - 1);							//얼굴도 프린트 해줌
					Text_color(0x0007);										//색 다시 흰색으로
					break;
				}
				Time_O = Timer; //시간 초기화
			}

			if (!Is_Apple_exist && MAX_Snake_length != 324) {		//사과가 맵에 없고 최대 길이가 아니면
				do {
					Apple_x = (rand() % 18) + 1;
					Apple_y = (rand() % 18) + 1;
				} while (Map[Apple_y][Apple_x] != 0);				//랜덤으로 뽑은 좌표가 빈칸인지 확인 후

				Object_move(Apple_x, Apple_y, Apple);				//사과 프린트 해준 후
				Is_Apple_exist = TRUE;								//기둥 뒤에 사과 있다고 해준다
			}

			if (_kbhit()) {						//키보드 입력이 들어오면
				key = _getch();						//키를 받고

				switch (key) {						//반대방향으로 가는게 아니면 키에 따라 방향을 바꿔준다
				case 77: if (Snake_direction_O != West && Is_Game_move) Snake_direction = East; break;
				case 75: if (Snake_direction_O != East && Is_Game_move) Snake_direction = West; break;
				case 80: if (Snake_direction_O != North && Is_Game_move) Snake_direction = South; break;
				case 72: if (Snake_direction_O != South && Is_Game_move) Snake_direction = North; break;

				case 13: Is_Game_move = FALSE; break;							//엔터를 입력받으면 게임을 잠시 멈춘다
				case 110:													//n을 누르면
					if (Is_Mode_Debug) Is_Mode_Noclip = 1 - Is_Mode_Noclip;	//디버그 모드라면 노클립 모드를 조정 가능
					Debug_Show();											//노클립 모든지 체크해줌
					break;
				}

				if (Is_Game_move == FALSE) {		//게임이 일시정지되면 다음처럼 출력해줌
					gotoxy(54, 9);
					printf("P A U S E");
					gotoxy(45, 11);
					printf("Continue\t\tGo To Menu");
					gotoxy(45, 12);
					printf(" [Enter] \t\t  [Space]");

					if (_kbhit) {			//키보드 입력을 받고
						key = _getch();

						switch (key) {
						case 13:						//엔터를 받으면
							Is_Game_move = TRUE;			//게임 계속 할거임
							gotoxy(54, 9);				//일시정지일때 출력됬던거 지워줌
							printf("                     ");
							gotoxy(45, 11);
							printf("                                ");
							gotoxy(45, 12);
							printf("                            ");
							break;

						case 32:						//스페이스면
							Is_Ingame = FALSE;				//게임 안할거임 ㅡㅡ 스네이크 꺼라
							Is_Pause_stop = TRUE;			//바로 꺼라
							gotoxy(54, 9);					//일시정지일때 출력됬던거 지워줌
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

			gotoxy(23, 21);														//밑으로 가서
			printf("점수 :    %4d 점\n", Snake_length*Difficulty_num);			//점수 프린트해줌

			if (Is_Mode_Debug) {												//디버그 할 때 테스트 코드임
				printf("MAX_Snake_length = %d\n", MAX_Snake_length);				//뱀 최대길이
				printf("Speed = %d (ms)\n", Difficulty_speed[Difficulty_num]);		//속도
				printf("Snake_direction = %d\n", Snake_direction);					//뱀 머리 방향
				printf("timer = %d", Timer);										//단위시간

				for (x = 0; x < 20; x++) {
					for (y = 0; y < 20; y++) {
						gotoxy(45 + 2 * x, y);										//게임 화면 옆에
						printf("%d ", Map[y][x]);									//그림이 아니라 맵의 값 자체를 출력
					}
				}
			}
		}

		//Is_Ingame이 0이면 (== 게임이 끝났으면)
		fflush(stdin);	//지금까지 입력받은 버퍼 다 지운다

		if (Is_Pause_stop) {			//일시정지해서 끝난 것이라면
			Snake_clear(Snake_length);		//뱀을 초기화
			Map[Apple_y][Apple_x] = 0;		//맵도 초기화
			Apple_x = Apple_y = 0;
		}

		else{							//정상적으로 종료된 것이면
			gotoxy(45, 5);																		//옆에다가
			char Gameover[12] = { 'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R','.','.','.' };	//게임 오버 글씨
			for (x = 0; x < 12; x++) {
				printf("%c ", Gameover[x]);														//gameover 글씨를 차례대로 띄워줌
				Sleep(100);																		//잠깐 멈춤
			}

			//게임 오버 화면을 출력해준다
			gotoxy(45, 9);
			printf("다시 플레이 하시겠습니까?");
			gotoxy(45, 11);
			printf("  Yes\t\t  No");
			gotoxy(45, 12);
			printf("[Enter]\t\t[Space]");

			if ((Difficulty_num < 6) && (Snake_length*Difficulty_num  >= 100)) { //지옥급 미만 난이도에서 100점 이상시
				gotoxy(45, 10);
				Text_color(0x0004);							//빨강
				printf("난이도 선택 창 + ↓×10 = ...?");	//지옥급 해금 힌트를 알려준다
				Text_color(0x0007);							//하양
			}

			BOOL Is_Game_question = TRUE;	//이 화면을 유지시킬 변수
			while (Is_Game_question) {		// 키 누르기 전까지 무한 루프,
				key = _getch();

				switch (key) {
				case 32:						//스페이스 누르면
					gotoxy(0, 23);					//게임 화면 밑으로 가서 (맨 마지막 종료 문구를 밑에 뜨게 하기 위함)
					Is_Game = FALSE;					//응 게임 꺼라
					Is_Game_question = FALSE;			//이 화면도 꺼라
					break;

				case 13:						//엔터 누르면
					Snake_clear(Snake_length);		//뱀을 초기화
					Map[Apple_y][Apple_x] = 0;		//맵도 초기화
					Apple_x = Apple_y = 0;			//사과 좌표도 초기화
					Is_Game_question = FALSE;		//이 화면도 꺼라
					break;
				}
			}
		}
	}
}

//좌표 구현 함수
void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

//커서 안깜빡이게 하는 함수
void CursorView(int show) {
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
}

//뱀 초기화 하는 함수
void Snake_clear(int n) {
	for (x = 0; x < n; x++) {
		Map[Snake_y[x]][Snake_x[x]] = 0;	//맵을 초기화함
		Snake_x[x] = Snake_y[x] = 0;		//뱀 배열도 초기화함
	}
	Snake_x[n] = Snake_y[n] = 0;			//맨 마지막 뱀도 초기화해줌
}

//맵 프린트 해주는 함수
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
	if (Is_Mode_Debug) {	//디버그 모드이면
		gotoxy(0, 0);			//화면 좌상단에
		Text_color(0x0003);		//파랑
		printf("ⓓ");			//디버그!
		Text_color(0x0007);		//하양
	}
	else {
		gotoxy(0, 0);			//화면 좌상단에
		printf("▩");			//응 일반 플레이야~
	}

	if (!(!Is_Mode_Debug || Is_Ingame)) {	//디버그 모드이면서 메뉴일때
		gotoxy(45, 3);						//설명문 출력
		printf("****디버그  모드****");
		gotoxy(45, 5);
		printf("[d] 디버그 모드 종료");
		gotoxy(45, 6);
		printf("[n] 노클립 모드 실행");
		gotoxy(45, 7);
		printf("[s] 속도 임의로 입력");
	}

	else {									//아니면
		gotoxy(45, 3);						//다 지워
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
		printf("ⓝ");
		Text_color(0x0007);
	}
	else {
		gotoxy(38, 0);
		printf("▩");
	}

	if (!(!Is_Mode_Noclip || Is_Ingame)) {
		gotoxy(61, 6);
		printf("종료");
	}
	else if (!(!Is_Mode_Debug || Is_Ingame)) {
			gotoxy(61, 6);
			printf("실행");
	}
}

//한 칸만 오브젝트를 바꿔주는 함수
void Object_move(int x, int y, int a) {
	Map[y][x] = a;
	gotoxy(2 * x, y);
	Object_print(a);
}

//오브젝트를 출력해주는 함수
void Object_print(int a) {
	switch (a) {
		case 0:
			puts("  ");
			break;

		case Apple:
			Text_color(0x000C);
			puts("●");
			Text_color(0x0007);
			break;

		case Wall:
			puts("▩");
			break;
	}
}

//특별 관리대상 뱀을 출력해주는 함수
void Snake_print(int n) {
	int Dx_Before, Dy_Before, Dx_After, Dy_After;	//각 블럭을 기준으로 전/후 블럭의 좌표 변화
	BOOL Up, Down, Left, Right;						//각 블럭을 기준으로 상하좌우에 블럭이 있는지 체크하는 변수
	Up = Down = Left = Right = FALSE;				//일단 이 블럭은 상하좌우에 뱀이 없습니다
	Map[Snake_y[n]][Snake_x[n]] = Snake;			//그리고 일단 이 좌표에는 뱀이 있습니다

	if (n != 0) {									//이게 꼬리가 아니라면
		Dx_Before = Snake_x[n] - Snake_x[n - 1];		// Dx = 1; 저번 칸이 왼쪽, Dx = -1; 저번 칸이 오른쪽
		Dy_Before = Snake_y[n] - Snake_y[n - 1];		// Dy = 1; 저번 칸이 아래쪽, Dy = -1; 저번 칸이 위쪽
	}
	else Dx_Before = Dy_Before = 0;					//꼬리면 이전 뱀이 없다고 알려줌

	if (Snake_x[n + 1] != 0) {						//이게 머리가 아니라면 (머리 길이 이후값의 좌표값은 0으로 설정했으므로)
		Dx_After = Snake_x[n] - Snake_x[n + 1];			// Dx = 1; 다음 칸이 왼쪽, Dx = -1; 다음 칸이 오른쪽
		Dy_After = Snake_y[n] - Snake_y[n + 1];			// Dy = 1; 다음 칸이 아래쪽, Dy = -1; 다음 칸이 위쪽
	}
	else Dx_After = Dy_After = 0;					//머리면 다음 뱀이 없다고 알려줌

	if (Dx_Before == 1 || Dx_After == 1)	Left = TRUE;	//왼쪽에 뱀이 있냐??
	if (Dx_Before == -1 || Dx_After == -1)	Right = TRUE;	//오른쪽
	if (Dy_Before == 1 || Dy_After == 1)	Up = TRUE;		//위쪽
	if (Dy_Before == -1 || Dy_After == -1)	Down = TRUE;	//아래쪽

	gotoxy(2 * Snake_x[n], Snake_y[n]);				//뱀 칸으로 가서

	//다음 설명부터 숫자는 키패드 기준임 (ex (24) = 상, 좌에 뱀이 있음, (20) == 상에만 뱀이 있음)
	if (Up) {							//위에 뱀이 있음
		if (Left) puts("┛");				//왼쪽에 뱀이 있음						(24)
		else if (Right) puts("┗");			//오른쪽에 뱀이 있음					(26)
		else puts("┃");					//아래에 뱀이 있던 없던 상관없이		(20, 28)
	}

	else if (Left) {					//왼쪽에 뱀이 있음
		if (Down) puts("┓");				//아래쪽에 뱀이 있음					(48)
		else puts("━");					//오른쪽에 뱀이 있던 없던 상관없이		(40, 46)
	}

	else if (Right) {					//오른쪽에 뱀이 있음
		if (Down) puts("┏");				//아래쪽에 뱀이 있음					(68)
		else printf("━");					//이미 상좌에 뱀이 없으므로				(60)
	}

	else if(Down) printf("┃");			//이미 상하좌에 뱀이 없으므로				(80)

	else printf("━");					//맨 처음 블록은 동쪽을 향하므로			(00)
}
*/