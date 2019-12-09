#include "main.h"

#define HURDLE_GAME_TIC_INTERVAL 10000

#define HOFFSET_X 2
#define HOFFSET_Y 22
#define HWIDTH 30
#define HHEIGHT 11

#define OBSTACLE_WIDTH 2
#define OBSTACLE_HEIGHT 3

#define TIC_PLAYER 10
#define TIC_DIFFICULT 2000

void hurdleTimerHandler(int sig);
int isInWidth(int x);
void makeNewObstacle(void);
void checkGameover(void);

unsigned int px = 3;	// x of Player
unsigned int py = 0;	// y of Player
double pv = 0;				// speed of Player(y)
unsigned int obtic = 10;	// tic of Obstacle moving
int obx[3] = {-100,-100,-100};	// x of Obstacle

void hurdleGame(int fd[])
{
	char ch;
	struct sigaction sa;
	struct itimerval tim;
	unsigned int space_num = 0;
	int i;

	sa.sa_handler = hurdleTimerHandler;
	sa.sa_flags = 0;

	if((sigemptyset(&sa.sa_mask) == -1) || (sigaction(SIGALRM, &sa, NULL) == -1))
		perror("Failed to install SIGALRM signal handler");

	tim.it_value.tv_sec = 0;
    tim.it_value.tv_usec = 5000;
    tim.it_interval.tv_sec = 0;
    tim.it_interval.tv_usec = 5000;
    
    if (setitimer(ITIMER_REAL, &tim, NULL) == -1)
	{
		perror("setitimer Error");
	}

	printf("\033[%dd\033[%dG@",HOFFSET_Y,HOFFSET_X-1);
	fflush(stdout);

	for (i=-1;i<=HWIDTH;i++)
	{
		printf("\033[%dd\033[%dG▩",HOFFSET_Y-1,HOFFSET_X+2*i+1);	//(i,-1)
		fflush(stdout);
		printf("\033[%dd\033[%dG▩",HOFFSET_Y+HHEIGHT,HOFFSET_X+2*i+1);	//(i,HHEIGHT)
		fflush(stdout);
	}

	for (i=-1;i<HHEIGHT-1;i++)
	{
		printf("\033[%dd\033[%dG▩",HOFFSET_Y+i+1,HOFFSET_X-1);	//(-1,i)
		fflush(stdout);
		printf("\033[%dd\033[%dG▩",HOFFSET_Y+i+1,HOFFSET_X+2*HWIDTH+1);	//(HWIDTH,i)
		fflush(stdout);
	}

	while(1)
	{
		while (read(fd[0], &ch ,1) == -1)
		{
			if (errno != EINTR)
			{
				perror("read function Error");
				return;
			}
		}

		if (ch == 32)
		{
			if (py==0) pv = 4;
		}
	}
}



void hurdleTimerHandler(int sig)
{
	static unsigned int tic = 1;
	unsigned int i, j;

	if (tic % obtic == 0)
	{
		for (i=0;i<3;i++)
		{
			if (isInWidth(obx[i]-1))
				for (j=HHEIGHT-OBSTACLE_HEIGHT;j<HHEIGHT;j++)
					printf("\033[%dd\033[%dG■",HOFFSET_Y + j, HOFFSET_X + 2*obx[i]-1);

			if (isInWidth(obx[i]+OBSTACLE_WIDTH-1))
				for (j=HHEIGHT-OBSTACLE_HEIGHT;j<HHEIGHT;j++)
					printf("\033[%dd\033[%dG  ", HOFFSET_Y + j, HOFFSET_X + 2*(obx[i]+OBSTACLE_WIDTH)-1);

			obx[i]--;
		}

		if (obx[0] < -OBSTACLE_WIDTH)
			makeNewObstacle();
	}

	if (tic % TIC_PLAYER == 0)
	{
		printf("\033[%dd\033[%dG  ", HOFFSET_Y + HHEIGHT-py-1, HOFFSET_X + 2*px+1);
		py = (py+pv>0) ? (unsigned int)(py+pv) : 0;
		pv = py ? pv-0.8 : 0;
		printf("\033[%dd\033[%dG◎", HOFFSET_Y + HHEIGHT-py-1,HOFFSET_X +  2*px+1);
	}

	if (tic % TIC_DIFFICULT == 0)
	{
		obtic = (obtic>3) ? obtic-1 : 1;
	}

	fflush(stdout);
	checkGameover();

	tic++;
}

int isInWidth(int x)
{
	if (x<0)
		return 0;
	if (x>=HWIDTH)
		return 0;
	return 1;
}

void makeNewObstacle(void)
{
	unsigned int i;

	if (obx[0] < -HWIDTH)
	{
		obx[0] = HWIDTH + rand()%20;
		for (i=1; i<3; i++)
			obx[i] = obx[i-1] + 30 + OBSTACLE_WIDTH + rand()%30;
		return;
	}

	for (i=0;i<2;i++)
		obx[i] = obx[i+1];
	obx[2] = obx[1] + 30 + OBSTACLE_WIDTH + rand()%30;
}

void checkGameover(void)
{
	unsigned int i;

	if (py>=OBSTACLE_HEIGHT)
		return;
	
	for (i=0; i<3; i++)
	{
		if ((px >= obx[i]) && (px < obx[i]+OBSTACLE_WIDTH))
			exit(1);
	}
}
