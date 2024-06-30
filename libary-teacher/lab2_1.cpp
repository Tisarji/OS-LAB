

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for exit()
#include <wait.h>   // for wait()

int main(){
	pid_t  pid;

	pid = fork();  //Fork a child process

        if(pid < 0){ //Fork error
		fprintf(stderr,"Fork failed.\n");
                exit(-1);
        }
        else if(pid==0){ // This is the path of child process
            printf("Before replace the child with other code...\n");
            execlp("/usr/bin/nano","nano",NULL); // call a text editor
	}
	else { // This is the path of parent process
	    printf("Before going into the wait  state...\n");
	    wait(NULL);
	    printf("Child process has terminated\n");
            exit(0);
        }
}
