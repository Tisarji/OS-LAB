#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// This example uses mutex to simulate a semaphore

char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;

//======================================================
#define SEM_MAX 256

typedef struct{
   int value;
   pthread_mutex_t mutex;
   pthread_cond_t condition;
}semaphore;

void sem_init(semaphore *sem,int value){
   sem->value = value;
   pthread_mutex_init(&(sem->mutex),NULL);
   pthread_cond_init(&(sem->condition),NULL);
}

void sem_destroy(semaphore *sem){
    pthread_cond_destroy(&(sem->condition));
    pthread_mutex_destroy(&(sem->mutex));
}

void sem_wait(semaphore *s){
	pthread_t pid;
	pthread_mutex_lock(&(s->mutex));
	s->value--;
        printf("value=%d\n",s->value);fflush(stdout);
	if(s->value < 0){
		printf("put a thread to wait\n");
		pthread_cond_wait(&(s->condition),&(s->mutex));
		pthread_mutex_unlock(&(s->mutex));

	}else{
		pthread_mutex_unlock(&(s->mutex));
	}
}

void sem_signal(semaphore *s){

	pthread_mutex_lock(&(s->mutex));
	s->value++;
	if(s->value <=0){
		pthread_cond_signal(&(s->condition));
		pthread_mutex_unlock(&(s->mutex));
		printf("continue a thread\n");
	} else
  	    pthread_mutex_unlock(&(s->mutex));
}

//======================================================

semaphore sem;

void randomDelay(void);
void serve(char *dest,char *src);
void *chefWork(void *who);

int main(void){
        int i;
	int param[5]={0,1,2,3,4};
	pthread_t tid[5];	// Thread ID
	pthread_attr_t attr[5];  	// Thread attributes

	isFull=10;		// Customer tell all chefs to stop making food
	// Locking mechanism,  the lock starts with false

	sem_init(&sem,1);

	for(i=0;i<4;i++)
		pthread_attr_init(&attr[i]);  // Get default attributes

	// Create 4 threads for producers
	for(i=0;i<4;i++)
		pthread_create(&tid[i],&attr[i],chefWork,(void *)&param[i]);

       // Wait until all threads finish
	for(i=0;i<4;i++)
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

    while(isFull>0){

	sem_wait(&sem);

	// Critical Section
	     	for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d is serving the food\n",chefNo+1);
		fflush(stdout);	

		for(j=0;j<3;j++)
		    serve(plate[j],chef[chefNo][j]);

     		for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d has serve the food\n",chefNo+1);
		fflush(stdout);	
	sem_signal(&sem);
	isFull--;
	randomDelay();
    }
    pthread_exit(0);
}