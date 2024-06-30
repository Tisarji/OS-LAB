#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "../../conio.h"
#include <semaphore.h>

// Try using one of these options to see how to recover deadlock
//#define _LOCK
//#define _TRYLOCK
#define _TIMEDLOCK



#if defined(_TIMEDLOCK)
struct timespec myTime;
#endif

char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;
int  isReady;

sem_t mutex;
sem_t display;

void randomDelay(void);
void randomDelay2(void);
void serve(char *dest,char *src);
void *chefWork(void *who);
void *customer(void *who);

int main(void){
        int i;
	int param[5]={0,1,2,3,4};
	pthread_t tid[5];	// Thread ID
	pthread_attr_t attr[5];  	// Thread attributes

	clrscr();

	isFull=false;		// Customer tell all chefs to stop making food
	isReady=false;		// Producer tell customer that food is ready
	// Locking mechanism,  the lock starts with false
	sem_init(&mutex,0,1);
	sem_init(&display,0,1);

	for(i=0;i<5;i++)
		pthread_attr_init(&attr[i]);  // Get default attributes

	// Create 4 threads for producers
	for(i=0;i<4;i++)
		pthread_create(&tid[i],&attr[i],chefWork,(void *)&param[i]);
	// Create 1 threads for consumer
	pthread_create(&tid[4],&attr[4],customer,(void *)&param[i]);

       // Wait until all threads finish
	for(i=0;i<5;i++)
		pthread_join(tid[i],NULL);

	sem_destroy(&mutex);
	sem_destroy(&display);

	return 0;
}

void randomDelay(void){
    int stime = ((rand()%10)+1)*1000;
    usleep(stime);
}

void randomDelay2(void){
      usleep(1000);	// Try comment this line to reduce the time between requesting two resources
}

void serve(char *dest,char *src){
   int i;
   srand(time(NULL));

   for(i=0;(i<63)&&(src[i]!=0);i++){
	dest[i]=src[i];
	randomDelay();
   }
   dest[i]=0;

}

void *chefWork(void *who){
    int plateNo,chefNo,i,j;

    chefNo = (int)*((int *)who);

    sem_wait(&display);
    gotoxy(1,chefNo*3+3);
    printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);	
    sem_post(&display);

    while(!isFull){

	if(isReady) {
           randomDelay(); // Wait for food to be taken
	}
	sem_wait(&mutex);
	randomDelay2();

#if defined(_TRYLOCK)
	if(sem_trywait(&display)){	// Try if the lock is a success
						// Return 0 is success
		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d Cannot lock the display     \n",chefNo+1);
		fflush(stdout);	
		sem_post(&mutex);	// Release the resource
		continue;		
	}
#elif defined(_TIMEDLOCK)
	time_t currentTime;
	time(&currentTime);  //Get current time
	myTime.tv_nsec=0;
	myTime.tv_sec=2+currentTime; // `2 seconds waiting from current time
	if(sem_timedwait(&display,&myTime)){	// Try if the lock is a success
						// Return 0 is success
		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d display timeout..            \n",chefNo+1);
		fflush(stdout);	
		sem_post(&mutex);	// Release the resource
		continue;		
	}
#else
	sem_wait(&display);
#endif
	if((!isFull)&&(!isReady)){
	// Critical Section
		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d is serving the food     \n",chefNo+1);
		fflush(stdout);	

		for(j=0;j<3;j++)
		    serve(plate[j],chef[chefNo][j]);
		isReady=true; //Tel customer that the food is ready

		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d has serve the food       \n",chefNo+1);
		fflush(stdout);	
	}
	sem_post(&display);
	sem_post(&mutex);

	if(isFull) break;
	randomDelay();
    }
    pthread_exit(0);
}

void *customer(void *who){
    int i,j;
    char dinner[256]={0};

    for(i=0;i<10;i++){

	while(!isReady) randomDelay();

	sem_wait(&display);
	randomDelay2();
	sem_wait(&mutex);
		// Critical Section
	gotoxy(1,1);
	printf("Choochok starts grab a set of dinner                                            \n");
	fflush(stdout);	

	serve(dinner,plate[0]); dinner[63]=0;
	serve(dinner + strlen(dinner),plate[1]); dinner[127]=0;
	serve(dinner + strlen(dinner),plate[2]); dinner[191]=0;

	gotoxy(1,1);
	printf("Plate NO:%d Choochok is eating %s                         \n",i+1,dinner);
	fflush(stdout);

        isReady=false;	// Food taken
	sem_post(&mutex);
	sem_post(&display);
    }
    isFull = true; // Tell other producer threads to stop;
    pthread_exit(0);
}
