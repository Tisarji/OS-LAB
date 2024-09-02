#include <pigpio.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <string.h>

void gpio_stop(int sig);
int running = 1;
#define  DHT22_PIN   17  // GPIO_17
#define  DHT22_DELAY 30  // Delay time for detecting 0 or 1
struct DHT22_data{
    float temp;
    float humidity;
};
#define udelay(us) gpioDelay(us)

int DHT22_Init();
int DHT22_read(struct DHT22_data *data);
#define DHT22_readOneByte(x)  {        \
   register int _i,_j;                  \
   for(_i=0;_i<8;_i++){                 \
        for(_j=0;_j<100;_j++){          \
            if(gpioRead(DHT22_PIN)==1)break; \
            udelay(1);                  \
        }                               \
        udelay(DHT22_DELAY);            \
        x <<=1;                         \
        if(gpioRead(DHT22_PIN))         \
            x|=1;                       \
        for(_j=0;_j<100;_j++){          \
            if(gpioRead(DHT22_PIN)==0)break; \
            udelay(1);                  \
        }                               \
   }}

int main(){
    struct DHT22_data data;
    DHT22_Init();
    signal(SIGINT,gpio_stop);
    while(running){
         if(DHT22_read(&data))
            printf("Temp = %5.1fc, Humidity = % 5.1f%%\r",data.temp,data.humidity);
         else printf("Error reading  data                               \r");
         fflush(stdout);
         sleep(1);
    }
	gpioTerminate();
    return 0;
}

int DHT22_Init(){
    if(gpioInitialise()<0) return -1;

	gpioSetMode(DHT22_PIN,PI_INPUT);
    gpioSetPullUpDown(DHT22_PIN,PI_PUD_OFF);
    return 0;
}

int DHT22_read(struct DHT22_data *data){
    int i;
    uint8_t temp_l,temp_h,hum_l,hum_h,crc;
    uint16_t temp,hum;
    //Sending Start signal
    hum_h=hum_l=temp_h=temp_l=crc=0;

	gpioSetMode(DHT22_PIN,PI_OUTPUT);
    gpioWrite(DHT22_PIN,0);
    udelay(1000);
    gpioWrite(DHT22_PIN,1);
    udelay(40);
    //waiting for response
    gpioSetMode(DHT22_PIN,PI_INPUT);
    for(i=0;i<100;i++){       // pull low cycle
        if(gpioRead(DHT22_PIN)==1) break;
        udelay(1);
    }
    for(i=0;i<100;i++){       //pull high cycle
        if(gpioRead(DHT22_PIN)==0) break;
        udelay(1);
    }
    // Read data
    DHT22_readOneByte(hum_h);
    DHT22_readOneByte(hum_l);
    DHT22_readOneByte(temp_h);
    DHT22_readOneByte(temp_l);
    DHT22_readOneByte(crc);

printf("hum_h = %.2X  hum_l = %.2X temp_h = %.2X temp_l = %.2X crc= %.2X\n",hum_h,hum_l,temp_h,temp_l,crc);
fflush(stdout);
    // Check if data is valid
    if(((hum_h+hum_l+temp_h+temp_l)&0xff)!=crc)
        return 0;

    hum = (((uint16_t)hum_h)<<8)|(uint16_t)hum_l;
    temp = (((uint16_t)temp_h)<<8)|(uint16_t)temp_l;
    data->humidity = (float)hum/10.0;
    if(temp&0x8000) data->temp = -((float)(temp&0x7fff))/10.0;
    else data->temp = (float)temp/10.0;
/*
    sprintf(tmp,"%u.%u",hum_h,hum_l);
    data->humidity = atof(tmp);
    sprintf(tmp,"%u.%u",temp_h,temp_l);
    data->temp = atof(tmp);
*/
    return 1;
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    running = 0;
}
