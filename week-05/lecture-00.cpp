#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int stop;
int ball;

void* player(void* selector);

int main(void)
{
	pthread_t tid[2];		// Thread ID
	pthread_attr_t attr[2];		// Thread attributes
	int section[2] = { 0,1 };
	stop = 0;
	ball = 0;
	char text[8];
	int round = 1;

	srand(time(NULL));

	pthread_attr_init(&attr[0]);	// Get default attributes
	pthread_attr_init(&attr[1]);	// Get default attributes

	// Create 2 threads
	pthread_create(&tid[0], &attr[0], player, (void*)&section[0]);
	pthread_create(&tid[1], &attr[1], player, (void*)&section[1]);

	while (1)
	{
		printf("[ROUND %d] Drop a ball...", round);
		ball = 1;
		printf("Press ENTER to drop a new ball or 0 to stop\n");
		fgets(text, 8, stdin);
		if (text[0] == '0')break;
		round++;
	}
	stop = 1;
	// Wait until all threads finish
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	return 0;
}

void* player(void* selector)
{
	int no = (int)*((int*)selector);
	int score = 0;

	while (1) {
		if (ball > 0) {
			printf("P%d Pick the ball\n", no + 1);
			usleep(rand() % 10000);
			ball = 0;
			score++;
		}
		usleep(rand() % 200000);
		if (stop) break;
	}
	printf("Player %d quits the game with score = %d\n", no + 1, score);
	pthread_exit(0);
}
