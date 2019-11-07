# TRI-TRY
이 프로그램은 2019년 시스템 프로그래밍(3428) Term Project를 위해 만들어 졌습니다.

## 파일 설명

### compile.c
이 파일은 만들 프로그램과 별도로 gcc컴파일을 편하게 하기 위해서 제작했다.
사용 방법은 다음과 같다.

1. compile.c를 단독으로 컴파일 한다.
    예시 : $gcc compile.c
2. 컴파일 후 실행파일을 실행할 경우 나머지 파일들이 컴파일 된다. (예> $gcc)
    예시 : $./a.out

### main.h
모든 소스코드가 공유하는 헤더 파일로, 표준 라이브러리와 모든 소스코드가 공유하는 함수를 정의한다.

### function.h
main.h에서 정의한 함수의 body가 있다.

### main.c, letter_game.c, snake_game.c, hurdle.c
각각의 프로세스가 동작하는 소스코드 파일이다.
