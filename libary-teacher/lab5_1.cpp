#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> // for usleep()
#include <stdlib.h> // for exit() and random generator
#include <wait.h>   // for wait()
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <time.h>

#define BUF_SIZE 4

struct shmareatype{
   int rp;
   int wp;
   int data[BUF_SIZE];
   int counter;
};

void randomDelay(void);
void consumer(struct shmareatype *shmarea);
void producer(struct shmareatype *shmarea);
int increment(int *counter);
int decrement(int *counter);

int main(){
        int shmid;  // Share memory ID
        struct shmareatype *shmarea; // Pointer to the shared segment
        int shmsize; // share segment size
        pid_t  pid;   // Child Process ID


        shmsize = sizeof(struct shmareatype);
        // Allocate the shared block from OS
        shmid = shmget(IPC_PRIVATE, shmsize, 0666 | IPC_CREAT );

        // Attach the shared block to the pointer so we can use it.
        shmarea = (struct shmareatype *)shmat(shmid, NULL, 0);

	shmarea->rp = shmarea->wp = shmarea->counter = 0;

        pid = fork();  //Fork a child process

        if(pid < 0){ //Fork error
                fprintf(stderr,"Fork failed.\n");
                exit(-1);
        }
        else if(pid==0){ // This is the path of child process
            consumer(shmarea);
        }
        else { // This is the path of parent process
            producer(shmarea);
            wait(NULL);
            printf("Child process has terminated\n");

          // Detach the shared memory segment
           shmdt(shmarea);
          // Remove the shared memory segment
           shmctl(shmid, IPC_RMID,NULL);
            exit(0);
        }
}

void consumer(struct shmareatype *shmarea){
    int i=0;
    int remaintime;

    while(1) // consume data
    {
       remaintime=5000; // Wait some seconds before quitting
       // Wait 5 seconds if no data in the circular buffer
       while((shmarea->counter==0)&&(remaintime>0)){
		    usleep(1000);remaintime--;
		    if(!(remaintime%1000))
		         printf("        Waiting for %d second(s).\n",remaintime/1000);
	   }
       if(remaintime==0) return;

       printf("        Data number:%d = %d\n",i++,shmarea->data[shmarea->rp]);
       shmarea->rp = ((shmarea->rp+1)%BUF_SIZE);
       decrement(&(shmarea->counter));
    }
}

void producer(struct shmareatype *shmarea){
    int i;

    for(i=0;i<32;i++) // produce data
    {
        while(shmarea->counter==BUF_SIZE) usleep(1000); //sleep for 1 ms if full
         printf("Enter data %d into share memory...\n",i);
         shmarea->data[shmarea->wp]=i;
       // move the write pointer so that the consumer know when to read.
       shmarea->wp = ((shmarea->wp+1)%BUF_SIZE);
       increment(&(shmarea->counter));
    }
}

void randomDelay(void){
// This function provides a delay which slows the process down so we can see what happens
   srand(time(NULL));
   int stime = ((rand()%1000)+100)*1000;
   usleep(stime);
}

int increment(int *counter){
// This function show how non-atomic instruction can cause problem in consumer-producer case
    int temp = *counter;printf("INC: read\n");
    randomDelay();
    temp = temp +1;printf("INC: ++\n");
    randomDelay();
    *counter = temp;printf("INC: write\n");
    return temp;
}

int decrement(int *counter){
// This function show how non-atomic instruction can cause problem in consumer-producer case
    int temp = *counter;printf("      DEC: read\n");
    randomDelay();
    temp = temp -1;printf("      DEC: --\n");
    randomDelay();
    *counter = temp;printf("      DEC: write\n");
    return temp;
}
