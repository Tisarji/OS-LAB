#include <pigpio.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
//#include <sched.h>

#define udelay(us) gpioDelay(us)
#define DHT11_PIN	17
#define DHT11_DELAY  79
//#define DHT11_DELAY	79
struct DHT11_data{
	float temp;
	float humidity;
}data;

int gpioLED[4] ={4,5,6,12};
int running=true;


void initGPIO();
void gpio_stop(int sig);
int piHiPri(const int pri);
//void udelay(const long us);

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

void *checkDistance(void *param);
void *showLED(void *param);

int main(){
    pthread_t tid[2];
    pthread_attr_t attr[2];
    void *(*thread[2])(void *)={checkDistance,showLED};
    int i;

    initGPIO();
    signal(SIGINT,gpio_stop);

    for(i=0;i<2;i++){
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i],&attr[i],thread[i],NULL);
    }

    printf("Waiting all threads to stop...\n");
    fflush(stdout);
    for(i=0;i<2;i++){
        pthread_join(tid[i],NULL);
    }
    for(i=0;i<2;i++){
        pthread_attr_destroy(&attr[i]);
    }
    gpioTerminate();
    return 0;
}

void initGPIO(){
    int i;

    if(gpioInitialise() < 0) exit(-1);

	gpioSetMode(DHT11_PIN,PI_INPUT);
    gpioSetPullUpDown(DHT11_PIN,PI_PUD_OFF);
	for(i=0;i<4;i++)
        gpioSetMode(gpioLED[i],PI_OUTPUT);
}

void *checkDistance(void *param){
	while(running){
		 if(DHT11_read(&data))
            printf("Temp = %5.1fc, Humidity = % 5.1f%%\r",data.temp,data.humidity);
         else printf("Error reading  data                               \r");
         fflush(stdout);
         usleep(200000);
	}
    pthread_exit(NULL);
}

void *showLED(void *param){
    while(running){
         if(data.temp<21) gpioWrite(gpioLED[0],0);
         else gpioWrite(gpioLED[0],1);
         if(data.temp<25) gpioWrite(gpioLED[1],0);
         else gpioWrite(gpioLED[1],1);
         if(data.temp<27) gpioWrite(gpioLED[2],0);
         else gpioWrite(gpioLED[2],1);
         if(data.temp<29) gpioWrite(gpioLED[3],0);
         else gpioWrite(gpioLED[3],1);
         usleep(100000);
    }
	pthread_exit(NULL);
}

void gpio_stop(int sig){
    printf("Exiting..., please wait\n");
    running = false;
}

int DHT11_read(struct DHT11_data *data){
    int i;
    uint8_t temp_l,temp_h,hum_l,hum_h,crc;
    char tmp[16];
    //Sending Start signal
    hum_h=hum_l=temp_h=temp_l=crc=0;

 //   piHiPri(50);

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
//    piHiPri(0);

printf("hum_h = %.2X  hum_l = %.2X temp_h = %.2X temp_l = %.2X crc= %.2X\n",hum_h,hum_l,temp_h,temp_l,crc);
fflush(stdout);
    // Check if data is valid
    if((hum_h+hum_l+temp_h+temp_l)!=crc)
        return 0;

    sprintf(tmp,"%u.%u",hum_h,hum_l);
    data->humidity = atof(tmp);
    sprintf(tmp,"%u.%u",temp_h,temp_l);
    data->temp = atof(tmp);

    return 1;
}

/*
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
*/
