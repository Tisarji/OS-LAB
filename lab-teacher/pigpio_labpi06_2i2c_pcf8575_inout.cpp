#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>

int running=true;
void gpio_stop(int sig);

int main(){
    int fd,i;
    uint16_t j;

    if(gpioInitialise() < 0) exit(-1);
    if((fd= i2cOpen(1,0x20,0)) < 0) exit(-1);
    signal(SIGINT,gpio_stop);

    printf("I2C  PCF8575 16bit GPIO In/Out testing...\n");
    while(running){
//    	i2cWriteQuick(fd,0);			 //Set command for writing data
//        i2cWriteByteData(fd,0,0xff);  //Set high state for acting as input
//		i2cWriteQuick(fd,1);		//Set command for reading
        j=i2cReadWordData(fd,0xff);
        printf("%.2X\n",j);
        j>>=8;
//        j&=0xfe;
        for(i=0;i<16;i++){
//			i2cWriteQuick(fd,0);   //Set command for writing
            i2cWriteByteData(fd,i&j,0xff);
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
