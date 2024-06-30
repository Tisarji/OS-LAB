#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

struct msgtype{
	long type;
	char data[256];
};

int main(){
	int qid;
	key_t key;
        char path[] = "/tmp";
        int id = 'a';
        struct msgtype message;

	printf("Message Reader\n==================\n");
	// Acquire a key using a path that user can access to.
	if((key = ftok(path,id))==-1){
		fprintf(stderr,"Error acquire a key...\n");
                return -1;
        }
        // Get the message queue ID and create it if it does not exist
	if((qid = msgget(key,IPC_CREAT | 0666))==-1){
		fprintf(stderr,"Error creating a message queue...\n");
		return -1;
	}

        printf("Waiting for messages :\n");
	do{
             if(msgrcv(qid,&message,256 // Size of data in the structure
				   ,0	// Accept all message  if this value is positive then get the exact type 
					// If it is negative it will receive only one less or equal to absolute value of type. 
				   ,0)==-1){   // Flag  this means  block receive 
		fprintf(stderr,"Error reading a message...\n");
		return -1;
 	     }
             printf("%s\n",message.data);
             if(message.type==2){
		// Exit the program
                printf("End transmittion...\n");
                return 0;
             }
        }while(1);
}
