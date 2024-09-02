#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int trig = 19;
int echo = 20;

int ledGPIO[4] = { 4,5,6,12 };
int runningSpeed = 1;

void	initGPIO();
int		ultraSonic();
void	*runningLED(void* param);
void	*getKey(void* param);
void	*check_distance(void* param);

void gpio_stop(int sig)
{
	printf("User pressing CTRL-C");
	gpioTerminate();
	exit(0);
}

int	main(void)
{
	pthread_t tid[3];
	pthread_attr_t attr[3];
	void* (*thread[3])(void*) = { runningLED,getKey,check_distance };
	int i;

	initGPIO();
	signal(SIGINT, gpio_stop);

	for (i = 0; i < 3; i++)
	{
		pthread_attr_init(&attr[i]);
		pthread_create(&tid[i], &attr[i], thread[i], NULL);
	}

	printf("Waiting all threads to stop...\n");
	fflush(stdout);
	for (i = 0; i < 3; i++)
		pthread_join(tid[i], NULL);

	// for (i = 0; i < 3; i++)
	// 	pthread_attr_destroy(&attr[i]);
	gpioTerminate();
	return 0;
}

void	initGPIO(void)
{
	if (gpioInitialise() < 0) exit(1);
	gpioSetMode(trig, PI_OUTPUT);
	gpioWrite(trig, 0);
	gpioSetMode(echo, PI_INPUT);
	gpioSetPullUpDown(echo, PI_PUD_OFF);
	sleep(2);

	/* Led */
	int i;
	if (gpioInitialise() < 0) exit(1);
	signal(SIGINT, gpio_stop);

	for (i = 0; i < 4; i++)
		gpioSetMode(ledGPIO[i], PI_OUTPUT);
}

int	ultraSonic(void)
{
	int clock1, clock2, timeout;
	gpioWrite(trig, 1);
	usleep(10);
	gpioWrite(trig, 0);
	for (timeout = 0; (timeout < 100000) && (gpioRead(echo) == 0); timeout++)usleep(10);
	clock1 = clock();
	for (timeout = 0; (timeout < 100000) && (gpioRead(echo) == 1); timeout++)usleep(10);
	clock2 = clock();
	return clock2 - clock1;
}

void	*check_distance(void *param)
{
	int err;
	while (1)
	{
		err = ultraSonic();
		if (err > 1000 || err < 100) continue;
		printf("check distance err varible: %d\n", err);
		runningSpeed = (err / 100);
		sleep(1);
	}
	pthread_exit(0);
}

/* Led running */
void	*runningLED(void *param)
{
	int i, j;
	int pattern[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };

	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
				gpioWrite(ledGPIO[j], pattern[i][j]);
			usleep(runningSpeed * 20000);
		}
		if ((runningSpeed > 9) || (runningSpeed < 1)) break;
	}
	pthread_exit(0);
}

void	*getKey(void* param)
{
	while (1)
	{
		printf("Running speed: %d\n", runningSpeed);
		//scanf("%d",&runningSpeed);
		//printf("Ultrasonic: %d\n", runningSpeed);
		if ((runningSpeed > 9) || (runningSpeed < 1)) break;
	}
	pthread_exit(0);
}
