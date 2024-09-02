
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
#define  DHT11_PIN   17  // GPIO_17
#define  DHT11_DELAY 79  // Delay time for detecting 0 or 1
struct DHT11_data{
    float temp;
    float humidity;
};

int piHiPri(const int pri);
void udelay(const long us);

int DHT11_Init();
int DHT11_read(struct DHT11_data *data);
#define DHT11_readOneByte(x)  {        \
   register int _i,_j;                  \
   for(_i=0;_i<8;_i++){                 \
        for(_j=0;_j<100;_j++){          \
            if(gpioRead(DHT11_PIN)==0)break; \
            udelay(1);                  \
        }                               \
        udelay(DHT11_DELAY);           \
        x <<=1;                         \
        if(gpioRead(DHT11_PIN))     \
            x|=1;                       \
   }}

int main(){
    struct DHT11_data data;
    DHT11_Init();
    signal(SIGINT,gpio_stop);
    while(running){
         if(DHT11_read(&data))
            printf("Temp = %5.1fc, Humidity = % 5.1f%%\r",data.temp,data.humidity);
         else printf("Error reading  data                               \r");
         fflush(stdout);
         sleep(1);
    }
	gpioTerminate();
    return 0;
}

int DHT11_Init(){
    if(gpioInitialise()<0) return -1;

	gpioSetMode(DHT11_PIN,PI_INPUT);
    gpioSetPullUpDown(DHT11_PIN,PI_PUD_OFF);
    return 0;
}

int DHT11_read(struct DHT11_data *data){
    int i;
    uint8_t temp_l,temp_h,hum_l,hum_h,crc;
    char tmp[16];
    //Sending Start signal
    hum_h=hum_l=temp_h=temp_l=crc=0;

    piHiPri(50);

	gpioSetMode(DHT11_PIN,PI_OUTPUT);
    gpioWrite(DHT11_PIN,0);
    usleep(18000);
    gpioWrite(DHT11_PIN,1);
    //waiting for response
    gpioSetMode(DHT11_PIN,PI_INPUT);
    for(i=0;i<100;i++){
        if(gpioRead(DHT11_PIN)==0) break;
        udelay(1);
    }
    for(i=0;i<100;i++){
        if(gpioRead(DHT11_PIN)==1) break;
        udelay(1);
    }
    // Read data
    DHT11_readOneByte(hum_h);
    DHT11_readOneByte(hum_l);
    DHT11_readOneByte(temp_h);
    DHT11_readOneByte(temp_l);
    DHT11_readOneByte(crc);
    piHiPri(0);

printf("hum_h = %.2X  hum_l = %.2X temp_h = %.2X temp_l = %.2X crc= %.2X\n",hum_h,hum_l,temp_h,temp_l,crc);
fflush(stdout);
    // Check if data is valid
    if(((hum_h+hum_l+temp_h+temp_l)&0xff)!=crc)
        return 0;

    sprintf(tmp,"%u.%u",hum_h,hum_l);
    data->humidity = atof(tmp);
    sprintf(tmp,"%u.%u",temp_h,temp_l);
    data->temp = atof(tmp);

    return 1;
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    running = 0;
}

int piHiPri(const int pri){
// Source code of this function by Gordon Henderson
    struct sched_param sched;
    memset(&sched,0,sizeof(sched));
    if(pri>sched_get_priority_max (SCHED_RR))
        sched.sched_priority = sched_get_priority_max (SCHED_RR);
        else
        sched.sched_priority = pri;
    return sched_setscheduler(0,SCHED_RR,&sched);
}

void udelay(const long us){
// Delay without yielding process
    long st;
    long tdif;
    struct timespec tnow;

    clock_gettime(CLOCK_REALTIME,&tnow);
    st = tnow.tv_nsec;
    while(1){
        clock_gettime(CLOCK_REALTIME,&tnow);
        tdif = tnow.tv_nsec - st;
        if(tdif < 0) tdif += 1000000000;
        if(tdif > (us*1000)) break;
    }
}
