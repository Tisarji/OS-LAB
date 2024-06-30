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

sem_t wrt;
sem_t mutex;
int readcount;

char data[1024];


void randomDelay(void);
void *writer(void *who);
void *reader(void *who);

int main(){
        pthread_t  tid[5];   // Child Process ID
	pthread_attr_t attr[5];
	int param[5]={0,1,2,3,4};
	int i;
	readcount=0;
// Initialize semaphores
	sem_init(&wrt,0,1);
	sem_init(&mutex,0,1);

	for(i=0;i<5;i++)
		pthread_attr_init(&attr[i]);

	for(i=0;i<4;i++)
		pthread_create(&tid[i],&attr[i],reader,(void *)&param[i]);
	pthread_create(&tid[4],&attr[4],writer,NULL);

	for(i=0;i<5;i++)
		pthread_join(tid[i],NULL);

	sem_destroy(&wrt);
	sem_destroy(&mutex);
	return 0;
}

void *reader(void *who){
    int readerNo=(int)*((int *)who);
    int i=0;

    for(int ii=0;ii<readerNo;ii++) randomDelay();
    while(1){
	sem_wait(&mutex);
	readcount++;
	if(readcount==1)
		sem_wait(&wrt);
	sem_post(&mutex);

		randomDelay();
       		printf("        (Consumer %d) Data is:%s\n",readerNo,data);fflush(stdout);
		for(int ii=0;ii<5;ii++)
            randomDelay();

	sem_wait(&mutex);
	readcount--;
	if(readcount==0)
		sem_post(&wrt);
	sem_post(&mutex);

// Remaining Section
      printf("       (Consumer %d) After exiting critical section\n",readerNo);fflush(stdout);
      if(data[0]=='0') break;
      randomDelay();
    }
    pthread_exit(0);
}

void *writer(void *who){
    int i=0;
    char temp[1024];

    while(1){
	printf("(Producer) Please enter a text (starting with 0 to stop) :");fflush(stdout);
	fgets(temp,1000,stdin);

	sem_wait(&wrt);

		printf("(Producer) Add %s into buffer\n",temp);fflush(stdout);
		for(i=0;i<1023;i++){
			if(temp[i]==0)break;
			data[i]=temp[i];
		}
		data[i]=0;
	sem_post(&wrt);

// Remaining Section
	printf("(Producer) After exiting critical section\n");fflush(stdout);
	if(temp[0]=='0')break;
    }
    pthread_exit(0);
}

void randomDelay(void){
// This function provides a delay which slows the process down so we can see what happens
   srand(time(NULL));
   int stime = ((rand()%2000)+100)*1000;
   usleep(stime);
}
