#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int x_global;
int y_global;

pthread_mutex_t	mutex_x;
pthread_mutex_t	mutex_y;

void showresult();
void *threadFunction1(void *selector);	//parameter must be void*0
void *threadFunction2(void *selector);
void *threadFunction3(void *selector);

int main(void)
{
	pthread_t tid1,tid2,tid3;			// Thread ID
	pthread_attr_t attr1,attr2,attr3;	// Thread attributes


	pthread_attr_init(&attr1);			// Get default attributes
	pthread_attr_init(&attr2);			// Get default attributes
	pthread_attr_init(&attr3);

	pthread_mutex_init(&mutex_x, NULL);
	pthread_mutex_init(&mutex_y, NULL);

	// Create 2 threads, share funcetion together.
	pthread_create(&tid1,&attr1,threadFunction1,NULL);
	pthread_create(&tid2,&attr2,threadFunction2,NULL);
	pthread_create(&tid3,&attr3,threadFunction3,NULL);

	// Wait until all threads finish
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	pthread_mutex_destroy(&mutex_x);
	pthread_mutex_destroy(&mutex_y);

	return (0);
}

void	*threadFunction1(void *selector)
{
	int count;
	int x_local;
	int	y_local;

	count = 0;
	x_local = 0;
	y_local = 0;
	while (1)
	{
		printf("Thread A Enter First: Number NO.[%d]  : ", count + 1);
		scanf("%d",&x_local);

		pthread_mutex_lock(&mutex_x);
		x_global = x_local;
		pthread_mutex_unlock(&mutex_x);

		printf("Thread A Enter Second: Number NO.[%d] : ", count + 1);
		scanf("%d",&y_local);

		pthread_mutex_lock(&mutex_y);
		y_global = y_local;
		pthread_mutex_unlock(&mutex_y);

		count++;
	}
}
void	*threadFunction2(void *selector)
{
	int count;

	count =  0;
	while (1)
	{
		sleep(3);
		pthread_mutex_lock(&mutex_x);
		printf("NO.[%d] Thread B Number: %d\n", count + 1, x_global);
		pthread_mutex_unlock(&mutex_x);
		count++;
	}
}
void *threadFunction3(void *selector)
{
	int count;

	count = 0;
	while (1)
	{
		if (count != 0)
			sleep(3);
		pthread_mutex_lock(&mutex_y);
		printf("NO.[%d] Thread C Number: %d\n", count + 1, y_global);
		pthread_mutex_unlock(&mutex_y);
		count++;
	}
}
