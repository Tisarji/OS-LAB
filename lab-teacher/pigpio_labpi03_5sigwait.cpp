#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int ledGPIO[4]={4,5,6,12};
void initGPIO();
pthread_t tid;
int count=5;

void *blinkingLED(void *param);

void gpio_stop(int sig){
    printf("User pressing CTRL-C (%d)\n",count);
	pthread_kill(tid,SIGUSR1);
	count--;
}

int main(){
    pthread_attr_t attr;
    int i;

    initGPIO();

    pthread_attr_init(&attr);
    pthread_create(&tid,&attr,blinkingLED,NULL);

    printf("Waiting LED thread to stop (using CTRL-C for 5 times)...\n");

    pthread_join(tid,NULL);
    pthread_attr_destroy(&attr);

	gpioTerminate();
    return 0;
}

void initGPIO(){
    int i;
    if(gpioInitialise() < 0) exit(1);
    signal(SIGINT,gpio_stop);
    for(i=0;i<4;i++)
        gpioSetMode(ledGPIO[i],PI_OUTPUT);
}

void *blinkingLED(void *param){
    int i,ii,j;
    int pattern[2][4]={{1,0,1,0},{0,1,0,1}};
    sigset_t signal_set;
    int sig;

    sigemptyset(&signal_set);
    sigaddset(&signal_set,SIGUSR1);

    while(count>0){
        for(ii=0;ii<3;ii++){
            for(i=0;i<2;i++){
                for(j=0;j<4;j++)
                    gpioWrite(ledGPIO[j],pattern[i][j]);
                usleep(250000);
            }
        }
        sigwait(&signal_set,&sig);
        printf("Getting signal %d\n",sig);
    }
    for(j=0;j<4;j++)
        gpioWrite(ledGPIO[j],0);
    pthread_exit(0);
}

