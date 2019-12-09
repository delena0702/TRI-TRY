#include <stdio.h>
#include <unistd.h>

int main(void)
{
	execlp("gcc", "gcc", "main.c", "function.c",
		"letter_game.c","snake_game.c","hurdle_game.c", "-o", "a", NULL);

	perror("Compile Fail");
	return -1;
}
