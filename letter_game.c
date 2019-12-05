#include "main.h"

#define HEIGHT 18
#define WIDTH 38

#define ALPHABET 26

void alarmSigHand(int sig);

void letterGame(int fd[])
{
	int input;
	char output;
	int y, x;
	int answer_count = 0;
	int signum = SIGALRM;
	struct itimerval current_timer;
	struct sigaction alrm_act;

	srand((unsigned int)time(NULL));

	initscr();
	raw();
	cbreak();
	noecho();

	output = rand() % ALPHABET + 'A';

	current_timer.it_interval.tv_sec = 5;	// sec_interval
	current_timer.it_interval.tv_usec = 0;	// microsec_interval
	current_timer.it_value.tv_sec = 5;
	current_timer.it_value.tv_usec = 0;
	
	if(setitimer(ITIMER_REAL, &current_timer , NULL) == -1)
	{
		perror("Failed to set timer");
		return 1;
	}

	alrm_act.sa_handler = alrmSigHand;
	alrm_act.sa_flags = 0;
	
	if(sigemptyset(&alrm_act.sa_mask) == -1 ||
		sigaction(SIGALRM, &alrm_act, NULL) == -1)
	{
		perror("Failed to install SIGALRM signal handler");
		return 1;
	}

	refresh();
	y = rand() % HEIGHT + 1;
	x = rand() % WIDTH + 1;

	win = newwin(HEIGHT + 2 , WIDTH + 2, 0, 0);
	box(win, 0, 0);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

	mvwprintw(win, y, x, "%c", output);
	wrefresh(win);

	while(input = fd[0][1])	// user_input
	{
		if(input == output || input == output - ('A' - 'a'))	
		{
			wclear(win);
			answer_count++;
			current_timer.it_value.tv_sec = 4 - (answer_count / 10);
			current_timer.it_value.tv_usec = 1000 - (answer_count * 100);

			if(setitimer(ITIMER_REAL, &current_timer , NULL) == -1)	// reset timer
			{
				perror("Failed to reset timer");
				exit(1);
				return 1;
			}
			else
			{
				box(win, 0, 0);
				wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
				wrefresh(win);
			}
			answer_count++;
			output = rand() % ALPHABET + 'A';
			y = rand() % HEIGHT + 1;
			x = rand() % WIDTH + 1;
			mvwprintw(win, y , x, "%c", output);
			move(0, 0);
			wrefresh(win);

		}
	}
	endwin();

	return 0;
}

void alrmSigHand(int sig)
{
	struct itimerval end_timer;
	struct sigaction end_act;

	end_timer.it_interval.tv_sec = 0;	// set end timer
	end_timer.it_interval.tv_usec = 0;
	end_timer.it_value.tv_sec = 5;
	end_timer.it_value.tv_usec = 0;

	sigfillset(&sigall);
	sigfillset(&sigmost);	// set sigmask
	sigdelset(&sigmost, SIGALRM);

	end_act.sa_handler = alrmSigHand;	//SETEND signal handler
	end_act.sa_flags = 0;

	if(sigaction(SIGALRM, &end_act, NULL) == -1)
	{
		perror("Failed to install SIGALRM signal handler");
		exit(1);	
		return ;
	}

	wclear(win);
	box(win, 0, 0);
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	mvwprintw(win, HEIGHT / 2, WIDTH / 2 , "Time Over!!");
	wrefresh(win);
	
	if(setitimer(ITIMER_REAL, &end_timer , NULL) == -1)	
	{
		perror("Failed to set endtimer");
		exit(1);
		return ;
	}
	exit(1);

}