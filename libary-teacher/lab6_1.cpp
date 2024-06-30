#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;
int  isReady;

sem_t sem;

void randomDelay(void);
void serve(char *dest,char *src);
void *chefWork(void *who);
void *customer(void *who);

int main(void){
        int i;
	int param[5]={0,1,2,3,4};
	pthread_t tid[5];	// Thread ID
	pthread_attr_t attr[5];  	// Thread attributes

	isFull=false;		// Customer tell all chefs to stop making food
	isReady=false;		// Producer tell customer that food is ready
	// Locking mechanism,  the lock starts with false

	sem_init(&sem,	// Semaphore
		0, 	// using only by threads of the process (nonzero value for other processes)
		1);	// Initial value

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

	sem_destroy(&sem);

	return 0;
}

void randomDelay(void){
    int stime = ((rand()%100)+1)*1000;
    usleep(stime);
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

    for(i=0;i<(chefNo+2);i++) printf("        ");
    printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);	

    while(!isFull){

	if(isReady) {
           randomDelay(); // Wait for food to be taken
	}

	sem_wait(&sem);

	if((!isFull)&&(!isReady)){
	// Critical Section
	     	for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d is serving the food\n",chefNo+1);
		fflush(stdout);	

		for(j=0;j<3;j++)
		    serve(plate[j],chef[chefNo][j]);
		isReady=true; //Tel customer that the food is ready

     		for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d has serve the food\n",chefNo+1);
		fflush(stdout);	
	}
	sem_post(&sem);

	if(isFull) break;
	// Remaining Section
	randomDelay();
    }
    pthread_exit(0);
}

void *customer(void *who){
    int i,j;
    char dinner[256]={0};

    for(i=0;i<10;i++){
	while(!isReady) randomDelay();

	sem_wait(&sem);
		// Critical Section
	printf("Choochok starts grab a set of dinner\n");
	fflush(stdout);	

	serve(dinner,plate[0]); dinner[63]=0;
	serve(dinner + strlen(dinner),plate[1]); dinner[127]=0;
	serve(dinner + strlen(dinner),plate[2]); dinner[191]=0;
	
	printf("Plate NO:%d Choochok is eating %s\n",i+1,dinner);
	fflush(stdout);	

        isReady=false;	// Food taken

	sem_post(&sem);
    }
    isFull = true; // Tell other producer threads to stop;
    pthread_exit(0);
}
