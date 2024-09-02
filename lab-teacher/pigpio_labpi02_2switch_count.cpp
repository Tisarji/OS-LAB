#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int key = 25;
int count = 0;

void *getKey(void *param);
void *displayKey(void *param);

int main(){
    int i;
	pthread_t tid[2];
    pthread_attr_t attr[2];
    void *(*thread[2])(void *)={getKey,displayKey};
    if(gpioInitialise()<0) return -1;

    gpioSetMode(key,PI_INPUT);
    gpioSetPullUpDown(key,PI_PUD_UP);
    printf("Set G25 as input\n");

    for(i=0;i<2;i++){
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i],&attr[i],thread[i],NULL);
    }
    for(i=0;i<2;i++){
        pthread_join(tid[i],NULL);
    }

    for(i=0;i<2;i++){
        pthread_attr_destroy(&attr[i]);
    }

    gpioTerminate();
    return 0;
}

void *getKey(void *param){
    while(count<10){
		while(gpioRead(key))
			usleep(100000); // wait for key press
		while(!gpioRead(key))
			usleep(100000); // wait for relasing key
		count++;
	}
	pthread_exit(NULL);
}
void *displayKey(void *param){
	while(count<10){
		printf("Key pressed : %2d\r",count);
		fflush(stdout);
		usleep(250000);
	}
	pthread_exit(NULL);
}





