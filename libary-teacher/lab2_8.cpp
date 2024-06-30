#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void consumer(int fds[]);
void producer(int fds[]);

int main(){
	int fds[2];  // Bidirectional pipe
        pid_t  pid;   // Child Process ID

	pipe(fds);

        pid = fork();  //Fork a child process

        if(pid < 0){ //Fork error
                fprintf(stderr,"Fork failed.\n");
                return -1;
        }
        else if(pid==0){ // This is the path of child process
            printf("Child process started..");
            close(fds[1]); // Close the unused end.(not send anything back to parent);
            consumer(fds);
            close(fds[0]); // This will close the read end.
        }
        else { // This is the path of parent process
            close(fds[0]); // Close the read back end. (Not receiving anything from child.
            producer(fds);
            wait(NULL);
            printf("Child process has terminated\n");
            close(fds[1]);  // Close the write end.
            return 0;
        }
}

void consumer(int fds[]){
    int i=0;
    FILE *fr;

    fr = fdopen(fds[0],"r");	// Open the write end to receive data

    for(i=0;i<16;i++){
       if(feof(fr)||ferror(fr)) sleep(2);
       printf("Data number:%d = %c\n",i,fgetc(fr));
    }
}

void producer(int fds[]){
    int i;
    char temp[16];
    FILE *fw;

    fw = fdopen(fds[1],"w");	// Open the write end to send data 

    for(i=0;i<16;i++) // produce data for 16 times
    {
	printf("Please enter a character No %d :",i);
        fgets(temp,16,stdin);
        printf("Enter data %c into pipe...\n",temp[0]);
        fputc(temp[0],fw);
        fflush(fw); // Flush data into stream
    }
}