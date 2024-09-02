#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int ledGPIO[4]={4,5,6,12};

void initGPIO();
void blinkingLED();

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    gpioTerminate();
    exit(0);
}

int main(){

    initGPIO();
    signal(SIGINT,gpio_stop);
    blinkingLED();
    gpioTerminate();
    return 0;
}

void initGPIO(){
    int i;
    if(gpioInitialise() < 0) exit(1);

    for(i=0;i<4;i++)
        gpioSetMode(ledGPIO[i],PI_OUTPUT);
}

void blinkingLED(){
   int i,j,k;
    int pattern[2][4]={{1,0,1,0},{0,1,0,1}};

    for(k=20;k>0;k--){
        for(i=0;i<2;i++){
            for(j=0;j<4;j++)
                gpioWrite(ledGPIO[j],pattern[i][j]);
			usleep(250000);
        }
        printf(" %d \r",k);
        fflush(stdout);
    }
    for(j=0;j<4;j++)
        gpioWrite(ledGPIO[j],0);
    printf("Program exits normally.");
}
