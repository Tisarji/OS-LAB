#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for exit()
#include <wait.h>   // for wait()
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>

struct shmareatype{
   int rp;
   int wp;
   char data[256];
};

void consumer(struct shmareatype *shmarea);
void producer(struct shmareatype *shmarea);

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

	shmarea->rp = shmarea->wp = 0;

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
    int i;

    for(i=0;i<16;i++) // consume data for 16 times
    {
       // Wait 2 minutes if no data in the circular buffer
       while(shmarea->wp==shmarea->rp) sleep(2);
       printf("Data number:%d = %c\n",i,shmarea->data[shmarea->rp]);
       shmarea->rp = ((shmarea->rp+1)%256);
    }
}

void producer(struct shmareatype *shmarea){
    int i;
    char temp[16];

    for(i=0;i<16;i++) // produce data for 16 times
    {
	printf("Please enter a character No %d :",i);
        fgets(temp,16,stdin);
        printf("Enter data %c into share memory...\n",temp[0]);
         shmarea->data[shmarea->wp]=temp[0];
       // move the write pointer so that the consumer know when to read.
       shmarea->wp = ((shmarea->wp+1)%256);
    }
}