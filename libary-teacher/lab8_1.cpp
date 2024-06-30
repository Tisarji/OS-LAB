#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int a;
thread_local int b;
void *threadFunction(void *selector);

int main(void){
	pthread_t tid[4];		// Thread ID
	pthread_attr_t attr[4];  	// Thread attributes
	int section[4] = { 0,1,2,3 };

    b = -1;
	for(int i=0;i<4;i++)
        pthread_attr_init(&attr[i]);  // Get default attributes
    for(int i=0;i<4;i++)
        pthread_create(&tid[i],&attr[i],threadFunction,(void *)&section[i]);

       // Wait until all threads finish
    for(int i=0;i<4;i++)
        pthread_join(tid[i],NULL);
	return 0;
}

void *threadFunction(void *selector){
	int sel=(int)*((int *)selector);

	b = sel;

	sleep(1);
	printf("b=%d\n",b);
	pthread_exit(0);
}
