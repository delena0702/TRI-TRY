#include "main.h"
#include<sys/ioctl.h>
#include<string.h>

#define HEIGHT 18
#define WIDTH 38

#define ALPHABET 26

void alarmSigHand(int sig);
int move_l(int x, int y);
// int move_s(int x, int y, int col)
// WINDOW *win;

void letterGame(int fd[])
{
	int input, output;
	int y, x, output_y, output_x;
	int answer_count = 0;
	int signum = SIGALRM;
	struct itimerval current_timer;
	struct sigaction alrm_act;

	srand((unsigned int)time(NULL));

	// initscr();
	// cbreak();
	// noecho();

	current_timer.it_interval.tv_sec = 0;	// sec_interval
	current_timer.it_interval.tv_usec = 0;	// microsec_interval
	current_timer.it_value.tv_sec = 5;
	current_timer.it_value.tv_usec = 0;
	
	if(setitimer(ITIMER_REAL, &current_timer , NULL) == -1)
	{
		perror("Failed to set timer");
		return ;
	}

	alrm_act.sa_handler = alarmSigHand;
	alrm_act.sa_flags = 0;
	
	if(sigemptyset(&alrm_act.sa_mask) == -1 ||
		sigaction(SIGALRM, &alrm_act, NULL) == -1)
	{
		perror("Failed to install SIGALRM signal handler");
		return ;
	}

	// struct winsize w;
   	// ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// refresh();
	// y = rand() % HEIGHT + 1;
	// x = rand() % HEIGHT + 1;

	// win = newwin(HEIGHT + 2 , WIDTH + 2, 0, 0);
	// box(win, 0, 0);
	// wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

	// mvwprintw(win, y, x, "%c", output);
	// wrefresh(win);

	char map[HEIGHT + 2][WIDTH + 2];
	for(y = 0; y < HEIGHT + 2; y++)
	{
		for(x = 0; x < WIDTH + 2; x++)
		{
			map[y][x] = ' ';
		}
	}

	for(x = 1; x < WIDTH + 2; x++)
	{
		map[0][x] = '-';
		map[HEIGHT + 1][x] = '-';
	}

	for(y = 0; y < HEIGHT + 2; y++)
	{
		map[y][0] = '|';
		map[y][WIDTH + 1] = '|';

	}

	output_y = rand() % HEIGHT + 1;
	output_x = rand() % WIDTH + 1;
	output = rand() % ALPHABET + 'A';

	map[output_y][output_x] = output;

	move_l(1, 1);
	for(y = 0; y < HEIGHT + 2; y++)
	{
		for(x = 0; x < WIDTH + 2; x++)
		{
			move_l(x + 1, y + 1);
			printf("%c", map[y][x]);
		}
		printf("\n");
	}

	while(true)	// user_input
	{
		int read_word = read(fd[0], &input, 1);

		if((read_word == 1) && (input == output || input == output - ('A' - 'a')))	
		{
			// wclear(win);
			map[output_y][output_x] = ' ';
			answer_count++;
			current_timer.it_value.tv_sec = 4 - (answer_count / 10);
			current_timer.it_value.tv_usec = 1000 - (answer_count * 100);

			if(setitimer(ITIMER_REAL, &current_timer , NULL) == -1)	// reset timer
			{
				perror("Failed to reset timer");
				exit(1);
				return ;
			}
			// else
			// {
			// 	box(win, 0, 0);
			// 	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
			// 	wrefresh(win);
			// }
			// answer_count++;
			output = rand() % ALPHABET + 'A';
			output_y = rand() % HEIGHT + 1;
			output_x = rand() % WIDTH + 1;

			map[output_y][output_x] = output;

			// mvwprintw(win, y , x, "%c", output);
			move_l(1, 1);
			for(y = 0; y < HEIGHT + 2; y++)
			{
				for(x = 0; x < WIDTH + 2; x++)
				{
					move_l(x + 1, y + 1);
					printf("%c", map[y][x]);
				}
				printf("\n");
			}

			// wrefresh(win);

		}
	}
	// endwin();

	return ;
}

int move_l(int x, int y)
{
	char essq[100];		// String variable to hold the escape sequence
	char xstr[100];		// Strings to hold the x and y coordinates
	char ystr[100];		// Escape sequences must be built with characters

	sprintf(xstr, "%d", x);
	sprintf(ystr, "%d", y);

	essq[0] = '\0';
	strcat(essq, "\033[");
	strcat(essq, ystr);

	strcat(essq, "d");
	
	strcat(essq, "\033[");
	strcat(essq, xstr);
	strcat(essq, "G");

	printf("%s", essq);

	return 0;
}

void alarmSigHand(int sig)
{
	// struct itimerval end_timer;
	// struct sigaction end_act;

	// end_timer.it_interval.tv_sec = 0;	// set end timer
	// end_timer.it_interval.tv_usec = 0;
	// end_timer.it_value.tv_sec = 5;
	// end_timer.it_value.tv_usec = 0;

	// end_act.sa_handler = alarmSigHand;	//SETEND signal handler
	// end_act.sa_flags = 0;

	// if(sigaction(SIGALRM, &end_act, NULL) == -1)
	// {
	// 	perror("Failed to install SIGALRM signal handler");
	// 	exit(1);	
	// 	return ;
	// }

	// wclear(win);
	// box(win, 0, 0);
	// wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	// mvwprintw(win, HEIGHT / 2, WIDTH / 2 , "Time Over!!");
	// wrefresh(win);
	
	// if(setitimer(ITIMER_REAL, &end_timer , NULL) == -1)	
	// {
	// 	perror("Failed to set endtimer");
	// 	exit(1);
	// 	return ;
	// }
	exit(1);

}