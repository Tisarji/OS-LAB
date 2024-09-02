#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int ledGPIO[4] = {4,5,6,12};

int main(){
    int i;
    if(gpioInitialise() < 0) exit(-1);
    for(i=0;i<4;i++)
        gpioSetMode(ledGPIO[i],PI_OUTPUT);
    for(i=0;i<4;i++)
        gpioWrite(ledGPIO[i],1);
    usleep(5000000);
    for(i=0;i<4;i++)
        gpioWrite(ledGPIO[i],0);

    gpioTerminate();
    return 0;
}
