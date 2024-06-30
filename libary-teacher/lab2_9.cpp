#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void consumer(void);
void producer(void);

char *pipeName="/tmp/myPipe";

int main(){
        pid_t  pid;   // Child Process ID

	system("mkfifo /tmp/myPipe");	// Create name pipe (FIFO)

        pid = fork();  //Fork a child process

        if(pid < 0){ //Fork error
                fprintf(stderr,"Fork failed.\n");
                return -1;
        }
        else if(pid==0){ // This is the path of child process
            printf("Child process started..");
            consumer();
        }
        else { // This is the path of parent process
            producer();
            wait(NULL);
            printf("Child process has terminated\n");
     	    system("rm /tmp/myPipe");  // Delete name pipe
            return 0;
        }
}

void consumer(){
    int i=0;
    FILE *fr;

    fr = fopen(pipeName,"r");	// Open the write end to receive data

    for(i=0;i<16;i++){
       if(feof(fr)||ferror(fr)) sleep(2);
       printf("Data number:%d = %c\n",i,fgetc(fr));
    }
    fclose(fr);
}

void producer(){
    int i;
    char temp[16];
    FILE *fw;

    fw = fopen(pipeName,"w");	// Open the write end to send data 

    for(i=0;i<16;i++) // produce data for 16 times
    {
	printf("Please enter a character No %d :",i);
        fgets(temp,16,stdin);
        printf("Enter data %c into pipe...\n",temp[0]);
        fputc(temp[0],fw);
        fflush(fw); // Flush data into stream
    }
    fclose(fw);
}