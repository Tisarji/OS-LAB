#include <pigpio.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void gpio_stop(int sig);
int running = 1;
#define  DS18B20_PIN   16  // GPIO_16
#define  udelay(us) gpioDelay(us)
int DS18B20_Init();
void DS18B20_Write(uint8_t data);
uint8_t DS18B20_Read(void);
uint8_t crc8(uint8_t *addr,uint8_t len);

int main(){
    uint8_t tempL,tempH;
    float temp;

    if(gpioInitialise()<0) return -1;

    signal(SIGINT,gpio_stop);
    while(running){
        if(!DS18B20_Init()){
            printf("No DS18B20 connected!\n");
            sleep(1);
            continue;
        }
        usleep(1000);
        DS18B20_Write(0xCC);  //Skip rom
        DS18B20_Write(0x44);  //Read temperature

        usleep(100000);
        if(!DS18B20_Init ()){
            printf("No DS18B20 connected!\n\r");
            sleep(1);
            continue;
        }
        usleep(1000);
        DS18B20_Write (0xCC);  // skip ROM
        DS18B20_Write (0xBE);  // read first two bytes from scratch pad
        {
            uint8_t scratchpad[9];
            uint8_t CRC=0;
            for(register uint8_t x=0;x<9;x++)
                scratchpad[x] = DS18B20_Read();
            CRC = crc8(scratchpad,8);
            printf ("CRC=%.2X scrCRC=%.2X   ",CRC,scratchpad[8]);
            tempL = scratchpad[0];
            tempH = scratchpad[1];
            if(CRC==scratchpad[8]){
 //       tempL = DS18B20_Read();
 //       tempH = DS18B20_Read();
            temp = ((float)((tempH<<8)|tempL))/16;
            printf("TH=%.2X TL=%.2X  Temperature=%f  \r",tempH,tempL,temp);
        }else{
            printf("Error reading temperature          \r");
        }
        }
        fflush(stdout);
        sleep(1);
    }
	gpioTerminate();
    return 0;
}

int DS18B20_Init(){
    uint8_t response=0;
    gpioSetMode(DS18B20_PIN,PI_OUTPUT);
    gpioWrite(DS18B20_PIN,0);
    udelay(480);    //Delay according to data sheet

    gpioSetMode(DS18B20_PIN,PI_INPUT);
    gpioSetPullUpDown(DS18B20_PIN,PI_PUD_OFF);
    udelay(80);     //Wait for DS18B20 to acknowledge;

    if(!gpioRead(DS18B20_PIN)) response = 1;
    udelay(480);    //Wait until DS18B20 ready to receive command
    return response;
}

void DS18B20_Write(uint8_t data){
	for (int i=0; i<8; i++)
	{
		gpioSetMode(DS18B20_PIN,PI_OUTPUT);  // set as output
		gpioWrite(DS18B20_PIN, 0);  // pull the pin LOW
		udelay (1);  // wait for 1 us
		if(data&1){
			gpioWrite(DS18B20_PIN, 1);  // pull the pin HIGH
			gpioSetMode(DS18B20_PIN,PI_INPUT); 	    //Release Low pull
		    udelay(60);  // wait for another 60 us   (1 us low + 59 us high pull floating)
		}else{
			udelay(60);	 // wait for 60-120 us according to datasheet (60 us low pull)
			gpioWrite(DS18B20_PIN, 1);  // pull the pin HIGH
			gpioSetMode(DS18B20_PIN,PI_INPUT); 	    //Release Low pull
		}
		data >>=1;
		udelay(5); // Wait for next bit
	}
    gpioSetMode(DS18B20_PIN,PI_INPUT); 	    //Release 1-wire bus
}

uint8_t DS18B20_Read(void){
	uint8_t value=0;

	for(int i=0;i<8;i++){
        gpioSetMode(DS18B20_PIN,PI_OUTPUT);  // set as output
		gpioWrite(DS18B20_PIN, 0);  // pull the pin LOW
        udelay(1);
		gpioWrite(DS18B20_PIN, 1);  // pull the pin HIGH
        gpioSetMode(DS18B20_PIN,PI_INPUT);  // master releases 1-wire bus

        value >>=1;
        udelay (5);  // wait for < 15us from the start of pulling LOW
        if(gpioRead(DS18B20_PIN))           // CHeck 1-wire status
	   		value |= 0x80;  // read = 1
	   udelay (60);  // wait for 50 us (The whole bit is atleast 60 us not including 1 us between bits)
	}
	return value;
}

uint8_t crc8(uint8_t *addr,uint8_t len){
    uint8_t crc = 0;
    while(len--){
        uint8_t inbyte = *addr++;
        for (uint8_t i=8; i; i--){
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if(mix) crc ^= 0x8c;
            inbyte >>=1;
        }
    }
    return crc;
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    running = 0;
}
