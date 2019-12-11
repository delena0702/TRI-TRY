#include "main.h"
#include<sys/ioctl.h>
#include<string.h>

#define HEIGHT 18
#define WIDTH 38

#define ALPHABET 26

void alarmSigHand(int sig);

void letterGame(int fd[])
{
	char input, output;
	int y, x, output_y, output_x;
	int answer_count = 0;
	int signum = SIGALRM;
	struct itimerval current_timer;
	struct sigaction alrm_act;

	srand((unsigned int)time(NULL));


	current_timer.it_interval.tv_sec = 0;	// sec_interval
	current_timer.it_interval.tv_usec = 0;	// microsec_interval
	current_timer.it_value.tv_sec = 10;
	current_timer.it_value.tv_usec = 0;
	
	if(setitimer(ITIMER_REAL, &current_timer , NULL) == -1)
	{
		perror("Failed to set timer");
		return ;
	}

	alrm_act.sa_handler = &alarmSigHand;
	alrm_act.sa_flags = 0;
	
	if(sigemptyset(&alrm_act.sa_mask) == -1 ||
		sigaction(SIGALRM, &alrm_act, NULL) == -1)
	{
		perror("Failed to install SIGALRM signal handler");
		exit(-1);
	}

	for(x = 1; x < WIDTH + 2; x++)
	{
		printf("\033[%dd\033[%dG-", 0, x);
		fflush(stdout);
		printf("\033[%dd\033[%dG-", HEIGHT + 2, x);
		fflush(stdout);
	}


	for(y = 0; y < HEIGHT + 3; y++)
	{
		printf("\033[%dd\033[%dG|", y, 0);
		fflush(stdout);
		printf("\033[%dd\033[%dG|", y, WIDTH + 2);
		fflush(stdout);

	}

	output_y = rand() % HEIGHT + 2;
	output_x = rand() % WIDTH + 2;
	output = rand() % ALPHABET + 'A';

	printf("\033[%dd\033[%dG%c",output_y ,output_x, output);
	fflush(stdout);


	while(1)	// user_input
	{
		int read_word = read(fd[0], &input, 1);
    
		if((read_word == 1) && (input == output || input == output - ('A' - 'a')))	
		{
			answer_count++;
			printf("\033[%dd\033[%dG%c",output_y ,output_x, ' ');

			int oldCycle = current_timer.it_value.tv_sec * 1000000 + current_timer.it_value.tv_usec;

			oldCycle *= 0.99;
			current_timer.it_value.tv_sec = oldCycle / 1000000;
			current_timer.it_value.tv_usec = oldCycle % 1000000;
			
			if(setitimer(ITIMER_REAL, &current_timer ,NULL) == -1)	// reset timer
			{
				perror("Failed to reset timer");
				exit(1);
			}
			output = rand() % ALPHABET + 'A';
			output_y = rand() % HEIGHT + 2;
			output_x = rand() % WIDTH + 2;
			
			for(x = 1; x < WIDTH + 2; x++)
			{
				printf("\033[%dd\033[%dG-", 0, x);
				fflush(stdout);
				printf("\033[%dd\033[%dG-", HEIGHT + 2, x);
				fflush(stdout);
			}


			for(y = 0; y < HEIGHT + 3; y++)
			{
				printf("\033[%dd\033[%dG|", y, 0);
				fflush(stdout);
				printf("\033[%dd\033[%dG|", y, WIDTH + 2);
				fflush(stdout);
			
			}

			printf("\033[%dd\033[%dG%c",output_y ,output_x, output);
			fflush(stdout);


		}
	}
}


void alarmSigHand(int sig)
{
	exit(0);
}
