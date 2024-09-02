#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int trig=19;
int echo=20;

void initGPIO();
int ultraSonic();

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    gpioTerminate();
    exit(0);
}


int main(){
    int delay;

    initGPIO();
    signal(SIGINT,gpio_stop);
    while(1){
        delay = ultraSonic();
        printf("Distance = %d\n",delay);
        fflush(stdout);
        sleep(1);
    }
    gpioTerminate();
    return 0;
}

void initGPIO(){
    if(gpioInitialise() <0) exit(1);

    gpioSetMode(trig,PI_OUTPUT);
    gpioWrite(trig,0);
    gpioSetMode(echo,PI_INPUT);
    gpioSetPullUpDown(echo,PI_PUD_OFF);
    sleep(2);
}

int ultraSonic(){
    int clock1,clock2,timeout;

    gpioWrite(trig,1);
    usleep(10);
    gpioWrite(trig,0);
    for(timeout=0;(timeout<100000)&&(gpioRead(echo)==0);timeout++)usleep(10);
    clock1=clock();
    for(timeout=0;(timeout<100000)&&(gpioRead(echo)==1);timeout++)usleep(10);
    clock2=clock();
    return clock2-clock1;
}
