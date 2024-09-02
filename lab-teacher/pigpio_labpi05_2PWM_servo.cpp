#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>

int PWM_pin=18;

void gpio_stop(int sig);

int main(){
    int i;

    printf("Servo PWM (5%%-10%% duty cycle)\n");

    if(gpioInitialise() < 0){
         return -1;
    }
    signal(SIGINT,gpio_stop);

    while(1){
        for(i=50;i<100;i++){
                gpioHardwarePWM(PWM_pin,50,i*1000);
                usleep(20000);
        }
        for(i=99;i>50;i--){
                gpioHardwarePWM(PWM_pin,50,i*1000);
                usleep(20000);
        }
    }
    return 0;
}

void gpio_stop(int sig){
    printf("User pressing CTRL-C");
    gpioTerminate();
    exit(0);
}
