#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int pwm=50;
int PWM_pin=18;
int running=true;

void gpio_stop(int sig);
void *servoPWM(void *param);

int main(){
    pthread_t tid;
    pthread_attr_t attr;
    int i;

    if(gpioInitialise() < 0) return -1;
    signal(SIGINT,gpio_stop);

    gpioSetMode(PWM_pin,PI_OUTPUT);
    pthread_attr_init(&attr);
    pthread_create(&tid,&attr,servoPWM,NULL);

    while(running){
        for(i=0;i<100;i++){
            pwm=i;
            if(!running)break;
            usleep(20000);
        }
        for(i=100;i>0;i--){
            pwm=i;
            if(!running)break;
            usleep(20000);
        }
    }

    pthread_join(tid,NULL);
    pthread_attr_destroy(&attr);
    gpioTerminate();
    return 0;
}

void *servoPWM(void *param){
    int pON,pOFF;
    while(running){
        pON = pwm*10+1000;
        pOFF = (100-pwm)*10+18000;
        gpioWrite(PWM_pin,1);
        usleep(pON);
        gpioWrite(PWM_pin,0);
        usleep(pOFF);
    }
    pthread_exit(0);
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    running=false;
}

