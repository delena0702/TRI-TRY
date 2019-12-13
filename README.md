# TRI-TRY
이 프로그램은 2019년 시스템 프로그래밍(3428) Term Project를 위해 만들어 졌습니다.
정상적인 실행을 위해서 창 크기를 가능한 크게하는 것을 권장합니다.

## 컴파일 방법
다음 명령어를 통해서 컴파일이 가능합니다.

```$ gcc main.c function.c letter_game.c snake_game.c hurdle_game.c```

## 파일 설명

### main.c
int main()이 있는 소스코드 파일이다.

### main.h
모든 소스코드가 공유하는 헤더 파일로, 표준 라이브러리와 모든 소스코드가 공유하는 함수를 정의한다.

### function.h
main.h에서 정의한 함수의 body가 있다.

### letter_game.c, snake_game.c, hurdle.c
각각의 미니게임이 동작하는 소스코드 파일이다.