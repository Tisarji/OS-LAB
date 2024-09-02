#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>

int PWM_pin=18;

void gpio_stop(int sig);

int main(){
    int i;

    printf("Servo PWM using Hardware Timed PWM(5%%-10%% duty cycle)\n");

    if(gpioInitialise() < 0){
         return -1;
    }
    signal(SIGINT,gpio_stop);

    gpioSetPWMfrequency(PWM_pin,50);
    gpioSetPWMrange(PWM_pin,20000);

    while(1){
        for(i=1000;i<2000;i+=10){
                gpioPWM(PWM_pin,i);
                usleep(10000);
        }
        for(i=2000;i>1000;i-=10){
                gpioPWM(PWM_pin,i);
                usleep(10000);
        }
    }
    return 0;
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    gpioTerminate();
    exit(0);
}
