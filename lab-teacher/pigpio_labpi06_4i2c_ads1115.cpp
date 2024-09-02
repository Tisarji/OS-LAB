#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>

int running=true;
void gpio_stop(int sig);

int main(){
    int fd;
	uint16_t data;
//	unit8_t writeBuf[3];
//	unit8_t readBuf[2];

    if(gpioInitialise() < 0) exit(-1);
    if((fd= i2cOpen(1,0x48,0)) < 0) exit(-1);
    signal(SIGINT,gpio_stop);

	//Start automatic conversion
//	writeBuf[0] = 1; //Select config register
//	writeBuf[2] = 0xc3; //  11000011   Single conversion / Ain0-GND / 4.096v gain / single-shot mode
//	writeBuf[3] = 0x03; //  00000011   8SPS / default comparator / active low-non-latching / disable comparator
/*
	i2cWriteWordData(fd,1,0xc303);
	while((i2cReadWordData(fd,1)&0x8000)==0){
		sleep(1000);
	}
*/
    printf("I2C  ADS1115 4ch ADC testing...\n");

    //Single-End input/read from port0 A/D on
    while(running){
    	i2cWriteWordData(fd,1,0x03c3);
	while(((data=i2cReadWordData(fd,1))&0x80)==0){
//        printf("CTRL = %.4X\n",data);
        usleep(100000);
	}
//	printf("CTRL = %.4X\n",data);
//	fflush(stdout);
//	break;
        data = i2cReadWordData(fd,0);
        //reverse LO/HI byte
        printf("Vout = %.4X\n",((data>>8)&0xff)|((data<<8)&0xff00));
        fflush(stdout);
        usleep(250000);
    }
    i2cClose(fd);
    gpioTerminate();
    return 0;
}

void gpio_stop(int sig){
    printf("Exiting..., please wait\n");
    running = false;
}
