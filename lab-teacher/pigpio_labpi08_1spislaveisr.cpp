#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <string.h>

#define SSPI_CE     21  //GPIO 21
#define SSPI_SCLK   24  //GPIO 24
#define SSPI_MOSI   22  //GPIO_22
#define SSPI_MISO   23  //GPIO_23
#define INFINITE	100000000

struct SSPI_type{
    unsigned char cpol;
    unsigned char cpha;
    unsigned char ready;
    unsigned char datain;
    unsigned char dataout;
}SSPI_data;

void gpio_stop(int sig);
int running = 1;

int SSPI_init();
void isr_SSPI_CE(int gpio,int level,uint32_t tick);
void isr_SSPI_SCLK(int gpio,int level,uint32_t tick);

int main(){
    unsigned char data[2],rdata[2];
    int count=0;
	int spi;

    // Slave side
    if(SSPI_init()<0) return -1;
	signal(SIGINT,gpio_stop);
    SSPI_data.cpha = 0;
    SSPI_data.cpol = 0;
    // Master side
	if(bbSPIOpen(8,9,10,11,5000,0)<0) return -1;

    while(running){
        data[0] = (unsigned char)count;
        printf("Master:Sending out data %.2X  ",data[0]);
		bbSPIXfer(8,(char *)data,(char *)rdata,1);
        printf("Master:Receiving in data %.2X\n",rdata[0]);
        count++;
        sleep(1);
        printf("Slave :Receiving in data %.2X\n",SSPI_data.datain);
        SSPI_data.dataout=SSPI_data.datain+3;
        sleep(1);
    }
	bbSPIClose(8);
	gpioTerminate();
    return 0;
}

int SSPI_init(){
    SSPI_data.cpol = SSPI_data.cpha = 0;
    SSPI_data.ready=0;

	if(gpioInitialise() < 0) return -1;

	gpioSetMode(SSPI_CE,PI_INPUT);
    gpioSetPullUpDown(SSPI_CE,PI_PUD_OFF);
    gpioSetMode(SSPI_SCLK,PI_INPUT);
    gpioSetPullUpDown(SSPI_SCLK,PI_PUD_OFF);
    gpioSetMode(SSPI_MOSI,PI_INPUT);
    gpioSetPullUpDown(SSPI_MOSI,PI_PUD_OFF);
    gpioSetMode(SSPI_MISO,PI_OUTPUT);

    if(gpioSetISRFunc(SSPI_CE,EITHER_EDGE,INFINITE,isr_SSPI_CE)<0) return -1;
    if(gpioSetISRFunc(SSPI_SCLK,EITHER_EDGE,INFINITE,isr_SSPI_SCLK)<0) return -1;

    return 0;
}

void isr_SSPI_CE(int gpio,int level,uint32_t tick){
	if(level==2) return;
    if(gpioRead(SSPI_CE))
//	if(level)
        SSPI_data.ready=1;
    else{
        SSPI_data.datain=0;
        if(!SSPI_data.cpha){
            if(SSPI_data.dataout&0x80)
                gpioWrite(SSPI_MISO,1);
            else
                gpioWrite(SSPI_MISO,0);
            SSPI_data.dataout <<= 1;
        }
    }

}

void isr_SSPI_SCLK(int gpio,int level,uint32_t tick){
	if(level==2) return;
    if(gpioRead(SSPI_CE))
        return;

    if(gpioRead(SSPI_SCLK)){
//	if(level){
        if(SSPI_data.cpha){
            if(SSPI_data.dataout&0x80)
                gpioWrite(SSPI_MISO,1);
            else
                gpioWrite(SSPI_MISO,0);
            SSPI_data.dataout <<= 1;
        }else{
            SSPI_data.datain <<=1;
            SSPI_data.datain |=gpioRead(SSPI_MOSI);
        }
    }else{
        if(SSPI_data.cpha){
            SSPI_data.datain <<=1;
            SSPI_data.datain |=gpioRead(SSPI_MOSI);
        }else{
            if(SSPI_data.dataout&0x80)
                gpioWrite(SSPI_MISO,1);
            else
                gpioWrite(SSPI_MISO,0);
            SSPI_data.dataout <<= 1;
        }
    }
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    running = 0;
}
