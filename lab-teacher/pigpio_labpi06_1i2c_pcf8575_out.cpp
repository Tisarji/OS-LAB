#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>

int running=true;
void gpio_stop(int sig);

int main(){
    int fd,i;

    if(gpioInitialise() < 0) exit(-1);
    if((fd= i2cOpen(1,0x20,0)) < 0) exit(-1);
    signal(SIGINT,gpio_stop);

    printf("I2C  PCF8575 16bit GPIO testing...\n");

//    i2cWriteByteData(fd,6,0);  //Set Port 0 as OUT

    while(running){
        for(i=0;i<16;i++){
            i2cWriteByteData(fd,i,0xff);
            usleep(100000);

        }
        printf("."); fflush(stdout);
    }
    i2cClose(fd);
    gpioTerminate();
    return 0;
}

void gpio_stop(int sig){
    printf("Exiting..., please wait\n");
    running = false;
}
