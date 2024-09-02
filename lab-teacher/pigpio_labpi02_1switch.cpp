#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int key = 25;

int main(){
    int ch;

    if(gpioInitialise()<0) return -1;

    gpioSetMode(key,PI_INPUT);
    gpioSetPullUpDown(key,PI_PUD_UP);
    printf("Set G25 as input\n");
	printf("Value = %d\n",gpioRead(key));
    gpioTerminate();
    return 0;
}






