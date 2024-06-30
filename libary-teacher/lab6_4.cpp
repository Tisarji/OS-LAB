#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> // for usleep() 
#include <stdlib.h> // for exit() and random generator
#include <wait.h>   // for wait()
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define BUF_SIZE 256
#define MAX_SIZE 255 

sem_t empty;
sem_t full;
sem_t mutex;

int rp;
int wp;
char data[BUF_SIZE];


void randomDelay(void);
void *consumer(void *who);
void *producer(void *who);

int main(){
        pthread_t  tid[2];   // Child Process ID
	pthread_attr_t attr[2];
	int i;

// Initialize semaphores 
	sem_init(&empty,0,MAX_SIZE);
	sem_init(&full,0,0);
	sem_init(&mutex,0,1);

	rp = wp = 0;

	for(i=0;i<2;i++)
		pthread_attr_init(&attr[i]);

	pthread_create(&tid[0],&attr[0],producer,NULL);
	pthread_create(&tid[1],&attr[1],consumer,NULL);

	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);

	sem_destroy(&empty);
	sem_destroy(&full);
	sem_destroy(&mutex);
}

void *consumer(void *who){
    int i=0;
    char d='a';
    int se,sf;

    while(1){
	if(!sem_getvalue(&full,&sf)) 
	    printf("        (Consumer) full=%d\n",sf);fflush(stdout);
	sem_wait(&full);
	sem_wait(&mutex);
		if(!sem_getvalue(&full,&sf)) 
		     printf("        (Consumer) full=%d\n",sf);fflush(stdout);
		d = data[rp]; 
       		printf("        (Consumer) Data number:%d = %c\n",i++,data[rp]);fflush(stdout);
       		rp = (rp+1)%BUF_SIZE;
		rp++;
	sem_post(&mutex);
	sem_post(&empty);

// Remaining Section
      printf("       (Consumer) After exiting critical section\n");fflush(stdout);
      if(d=='0') break;
      randomDelay();
	randomDelay();
    }
    pthread_exit(0);
}

void *producer(void *who){
    int i=0;
    int se,sf;
    char temp[40];

    while(1){
	printf("(Producer) Please enter a character :");fflush(stdout);
	fgets(temp,40,stdin);
	if(!sem_getvalue(&empty,&se)) 
	    printf("(Producer) empty=%d\n",se);fflush(stdout);

	sem_wait(&empty);
	sem_wait(&mutex);
    		if(!sem_getvalue(&empty,&se)) 
		    printf("(Producer) empty=%d\n",se);fflush(stdout);
		printf("(Producer) Add %c into buffer\n",temp[0]);fflush(stdout);
        	data[wp]=temp[0];
       		// move the write pointer so that the consumer know when to read.
        	wp = (wp+1)%BUF_SIZE;
		wp++;
	sem_post(&mutex);
	sem_post(&full);

// Remaining Section
	printf("(Producer) After exiting critical section\n");fflush(stdout);
	if(temp[0]=='0')break;
	randomDelay();
    }
    pthread_exit(0);
}

void randomDelay(void){
// This function provides a delay which slows the process down so we can see what happens
   srand(time(NULL));
   int stime = ((rand()%1000)+100)*1000;
   usleep(stime);
}
