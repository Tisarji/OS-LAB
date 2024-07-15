#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// Copyright 2011 by T.Sripramong. You can use this code for educational purpose only.
// This program shows how to use 3 techniques provided by Linux
// Spinlock is a basic mechanism for critical section where a thread can
// access the critical section while other threads have to be waiting in a loop (spinning)
// Read/Write Lock is another lock mechanism whereby multiple reads from several threads are
// allowed but only one writer allowed.
// Mutex is a general mechanism for critical section which is generally found in other OSes. 

#define USE_SPINLOCK
#define USE_RWLOCK
#define USE_MUTEX


char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."} };
int  isFull;
int  isReady;

#if defined(USE_SPINLOCK)
pthread_spinlock_t  spinlock;
#elif defined(USE_RWLOCK)
pthread_rwlock_t rwlock;
#else
pthread_mutex_t mutex;
#endif

typedef struct s_data
{
	int	num1;
	int	num2;
} t_data;

void randomDelay(void);
void serve(char* dest, char* src);
void* chefWork(void* who);
void* customer(void* who);

int main(void)
{
	int i;
	int param[5] = {0, 1, 2, 3, 4};
	pthread_t tid[5];	// Thread ID
	pthread_attr_t attr[5];  	// Thread attributes

	isFull = false;		// Customer tell all chefs to stop making food
	isReady = false;		// Producer tell customer that food is ready
	// Locking mechanism,  the lock starts with false
#if defined(USE_SPINLOCK)
	pthread_spin_init(&spinlock, 0);
#elif defined(USE_RWLOCK)
	pthread_rwlock_init(&rwlock, NULL);
#else
	pthread_mutex_init(&mutex, NULL);
#endif

	for (i = 0;i < 5;i++)
		pthread_attr_init(&attr[i]);  // Get default attributes

	// Create 4 threads for producers
	for (i = 0;i < 4;i++)
		pthread_create(&tid[i], &attr[i], chefWork, (void*)&param[i]);
	// Create 1 threads for consumer
	pthread_create(&tid[4], &attr[4], customer, (void*)&param[i]);

	// Wait until all threads finish
	for (i = 0;i < 5;i++)
		pthread_join(tid[i], NULL);

#if defined(USE_SPINLOCK)
	pthread_spin_destroy(&spinlock);
#elif defined(USE_RWLOCK)
	pthread_rwlock_destroy(&rwlock);
#else
	pthread_mutex_destroy(&mutex);
#endif

	return 0;
}

void randomDelay(void)
{
	int stime = ((rand() % 100) + 1) * 1000;
	usleep(stime);
}

void serve(char* dest, char* src)
{
	int i;
	srand(time(NULL));

	for (i = 0;(i < 63) && (src[i] != 0);i++)
	{
		dest[i] = src[i];
		randomDelay();
	}
	dest[i] = 0;

}

void* chefWork(void* who)
{
	int plateNo, chefNo, i, j;

	chefNo = (int)*((int*)who);

	for (i = 0; i < (chefNo + 2); i++)
		printf("        ");
	printf("Chef NO %d start working...\n", chefNo + 1);
	fflush(stdout);

	while (!isFull)
	{
		if (isReady)
			randomDelay(); // Wait for food to be taken

#if defined(USE_SPINLOCK)
		pthread_spin_lock(&spinlock);
#elif defined(USE_RWLOCK)
		pthread_rwlock_wrlock(&rwlock);
#else
		pthread_mutex_lock(&mutex);
#endif

		if ((!isFull) && (!isReady))
		{
			// Critical Section
			for (i = 0; i < (chefNo + 2); i++)
			printf("      ");
			printf("Chef NO %d is serving the food\n", chefNo + 1);
			fflush(stdout);

			for (j = 0;j < 3;j++)
				serve(plate[j], chef[chefNo][j]);
			isReady = true; //Tel customer that the food is ready

			for (i = 0;i < (chefNo + 2);i++) printf("      ");
			printf("Chef NO %d has serve the food\n", chefNo + 1);
			fflush(stdout);
		}
		else
		{
					// for(i=0;i<(chefNo+2);i++) printf("      ");
					// printf("Chef NO %d has to skip the searving\n",chefNo+1);
					// fflush(stdout);	
		}

#if defined(USE_SPINLOCK)
		pthread_spin_unlock(&spinlock);
#elif defined(USE_RWLOCK)
		pthread_rwlock_unlock(&rwlock);
#else
		pthread_mutex_unlock(&mutex);
#endif

		if (isFull)
			break;
		// Remaining Section
		randomDelay();
		//	sleep(300000*(chefNo+1));  // This will cause some chef to waiting indefinitly
		//     	for(i=0;i<(chefNo+2);i++) printf("      ");
		//	printf("Chef NO %d has rested\n",chefNo+1);
		//	fflush(stdout);	
	}
	pthread_exit(0);
}

void* customer(void* who)
{
	int i, j;
	char dinner[256] = { 0 };

	for (i = 0;i < 10;i++)
	{
		//	printf("Choochok waits for food to be served\n");
		//	fflush(stdout);

		while (!isReady)
			randomDelay();

#if defined(USE_SPINLOCK)
		pthread_spin_lock(&spinlock);
#elif defined(USE_RWLOCK)
		pthread_rwlock_rdlock(&rwlock);
#else
		pthread_mutex_lock(&mutex);
#endif
		// Critical Section
		printf("Choochok starts grab a set of dinner\n");
		fflush(stdout);

		serve(dinner, plate[0]); dinner[63] = 0;
		serve(dinner + strlen(dinner), plate[1]); dinner[127] = 0;
		serve(dinner + strlen(dinner), plate[2]); dinner[191] = 0;

		printf("Plate NO:%d Choochok is eating %s\n", i + 1, dinner);
		fflush(stdout);

		isReady = false;	// Food taken
#if defined(USE_SPINLOCK)
		pthread_spin_unlock(&spinlock);
#elif defined(USE_RWLOCK)
		pthread_rwlock_unlock(&rwlock);
#else
		pthread_mutex_unlock(&mutex);
#endif
		// Remaining Section
	//	for(j=0;j<10;j++) randomDelay();
	//	printf("Choochok is moving to the next table\n");
	//	fflush(stdout);	
	}
	isFull = true; // Tell other producer threads to stop;
	pthread_exit(0);
}
