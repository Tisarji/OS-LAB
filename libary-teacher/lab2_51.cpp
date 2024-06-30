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

	printf("Message Sender\n==================\n");
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

	do{
             printf("Enter a string or just press ENTER to stop sending :");
             fgets(message.data,256,stdin);
	     if(message.data[0]<0x20) message.type=2;  // Notify reader to stop process 
             else  message.type=1; // Notify reader to read the message and display it. 
 
		// 256 here is the data size and 0 means WAIT until the message is sent
             if(msgsnd(qid,&message,256,0)==-1){ 
		fprintf(stderr,"Error sending a message...\n");
		  msgctl(qid,IPC_RMID,NULL); //Remove message queue
		return -1;
 	     }
        }while(message.type!=2);
        msgctl(qid,IPC_RMID,NULL); //Remove message queue
}
