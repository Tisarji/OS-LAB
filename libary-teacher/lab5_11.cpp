#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;
int  lock;
int  waiting[4];

void randomDelay(void);
void serve(char *dest,char *src);
void *chefWork(void *who);

int main(void){
        int i;
	int param[5]={0,1,2,3,4};
	pthread_t tid[5];	// Thread ID
	pthread_attr_t attr[5];  	// Thread attributes

	isFull=20;		// Number of disk remains
	lock = false;		// Locking mechanism
	for(i=0;i<5;i++) waiting[i]=false;

// No consumer!!!
	for(i=0;i<4;i++)
		pthread_attr_init(&attr[i]);  // Get default attributes

	// Create 4 threads for producers
	for(i=0;i<4;i++)
		pthread_create(&tid[i],&attr[i],chefWork,(void *)&param[i]);

// No consumer!!!
       // Wait until all threads finish
	for(i=0;i<4;i++)
		pthread_join(tid[i],NULL);
	return 0;
}

void randomDelay(void){
    int stime = ((rand()%50)+1)*1000;
    usleep(stime);
}

void serve(char *dest,char *src){
   int i;
   srand(time(NULL));

   for(i=0;(i<63)&&(src[i]!=0);i++){
	dest[i]=src[i];
	randomDelay();
   }
   dest[i]=0;

}

void *chefWork(void *who){
    int plateNo,chefNo,i,j;
    int key;

    chefNo = (int)*((int *)who);

    for(i=0;i<(chefNo+2);i++) printf("        ");
    printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);

    while(isFull>0){

	waiting[chefNo]=true;
	key = true;
	while(waiting[chefNo] && key){
		key = __sync_fetch_and_or(&lock,1);
		usleep(300000);
		if(isFull<=0) break;
	}
	waiting[chefNo]=false;
	// Critical Section
	if(isFull>0){
	     	for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d is serving the food\n",chefNo+1);
		fflush(stdout);

		for(j=0;j<3;j++)
		    serve(plate[j],chef[chefNo][j]);

     		for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d has serve the food\n",chefNo+1);
		fflush(stdout);
		isFull--;
	}

	j = (chefNo+1)%4;
	while((j!=chefNo)&& !waiting[j])
		j = (j+1)%4;
	if(j==chefNo){
		lock = false;
		printf("Chef No %d has unlocked the key.[%d][%d][%d][%d]\n",j+1,waiting[0],waiting[1],waiting[2],waiting[3]);
	}
	else{
		waiting[j]=false;
		printf("Passing to chef No %d [%d][%d][%d][%d]\n",j+1,waiting[0],waiting[1],waiting[2],waiting[3]);
	}
	// Remaining Section
	usleep((1000*(rand()%4000))+1);
    }
    pthread_exit(0);
}
