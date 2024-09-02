#include <pigpio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int key = 27;
int count = 0;

void isrFunc(int gpio,int level,uint32_t tick);

int main(){
    if(gpioInitialise()<0) return -1;

    gpioSetMode(key,PI_INPUT);
    gpioSetPullUpDown(key,PI_PUD_UP);
    printf("Set G27 as input\n");

    gpioSetISRFunc(key,FALLING_EDGE,10000,isrFunc);

    while(count<10){
        printf("Key pressed : %2d\r",count);
        fflush(stdout);
        usleep(250000);
    }

    gpioTerminate();
    return 0;
}

void isrFunc(int gpio,int level,uint32_t tick){
   count++;
}



