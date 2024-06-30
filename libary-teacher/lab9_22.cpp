#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for exit()
#include <wait.h>   // for wait()
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>

// This example create a real file and work as memory-mapped file
struct shmareatype{
   int rp;
   int wp;
   char data[256];
};

void consumer(struct shmareatype *shmarea);
void producer(struct shmareatype *shmarea);

int main(){
        int fd;  // Share memory ID
        struct shmareatype *shmarea; // Pointer to the shared segment
        int shmsize; // share segment size
        pid_t  pid;   // Child Process ID

        shmsize = sizeof(struct shmareatype);
        //Create new file to be mapped
        //!!! must link with rt  (-lrt)

        fd = shm_open("/tmp/temp_lab9_22",  // File name and location
                       O_RDWR|O_CREAT|O_TRUNC, // Read/write, create new, and cut to size
                       0600); //Accessing attribute

        // Allocate the shared block from OS
        shmarea= (struct shmareatype *)mmap(0,  //Let OS decide the address
                                            shmsize,
                                            PROT_READ|PROT_WRITE,
                                            MAP_SHARED | MAP_ANONYMOUS, // Shared between processes
                                            fd,  //Unnamed file = no permanent file opened
                                            0);  // offset of file location to work on

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

            munmap(shmarea,shmsize); // Un-mapping file
			close(fd);
			shm_unlink("/tmp/temp_lab9_22");
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
