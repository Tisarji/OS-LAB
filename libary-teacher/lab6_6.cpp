#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

#define BUF_SIZE 256
#define MAX_SIZE 255 

HANDLE empty,full,mutex;

DWORD sem_wait(HANDLE sem);
DWORD sem_signal(HANDLE sem);

int rp;
int wp;
char data[BUF_SIZE];

void randomDelay(void);
DWORD WINAPI consumer(LPVOID who);
DWORD WINAPI producer(LPVOID who);

int main(void){
	int i;
	DWORD tid[2];				// Thread ID
	HANDLE th[2];				// Thread Handle
	int param[]={0,1};

	rp = wp =0;

	empty = CreateSemaphore(
		NULL,   // default security attributes
		MAX_SIZE,   // initial count
		MAX_SIZE,   // maximum count
		NULL);  // unnamed semaphore
	full = CreateSemaphore(
		NULL,   // default security attributes
		0,   // initial count
		MAX_SIZE,   // maximum count
		NULL);  // unnamed semaphore

	mutex = CreateSemaphore(
		NULL,   // default security attributes
		1,   // initial count
		1,   // maximum count
		NULL);  // unnamed semaphore

	// Create 5 threads
	th[0] = CreateThread(
		NULL,				// Default security attributes
		0,					// Default stack size
		producer,			// Thread function
		(void *)&param[0],	// Thread function parameter
		0,					// Default creation flag
		&tid[0]);			// Thread ID returned.

	th[1] = CreateThread(
		NULL,				// Default security attributes
		0,					// Default stack size
		consumer,			// Thread function
		(void *)&param[1],	// Thread function parameter
		0,					// Default creation flag
		&tid[1]);				// Thread ID returned.

       // Wait until all threads finish
	for(i=0;i<2;i++)
		if(th[i]!=NULL)
			WaitForSingleObject(th[i],INFINITE);
	CloseHandle(empty);
	CloseHandle(full);
	CloseHandle(mutex);
	return 0;
}

void randomDelay(void){
    int stime = ((rand()%1000)+100);
    Sleep(stime);
}


DWORD WINAPI producer(LPVOID who){
	char temp[40];

	while(1){
		printf("(Producer) Please enter a character :");fflush(stdout);
		fgets(temp,40,stdin);
   
	sem_wait(empty);
	sem_wait(mutex);
	// Critical Section
        printf("(Producer) Add %c into buffer\n",temp[0]);fflush(stdout);
        data[wp]=temp[0];
       	// move the write pointer so that the consumer know when to read.
        wp = (wp+1)%BUF_SIZE;
//		wp++;

	sem_signal(mutex);
	sem_signal(full);
	// Remaining Section

	printf("(Producer) After exiting critical section\n");fflush(stdout);
	if(temp[0]=='0')break;
//	randomDelay();
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID who){
    int i=0;
	char d='a';

    while(1){
	sem_wait(full);
	sem_wait(mutex);
	// Critical Section

		d = data[rp]; 
       	printf("        (Consumer) Data number:%d = %c\n",i++,data[rp]);fflush(stdout);
       	rp = (rp+1)%BUF_SIZE;
//		rp++;

	sem_signal(mutex);
	sem_signal(empty);

	// Remaining Section
	printf("       (Consumer) After exiting critical section\n");fflush(stdout);
    if(d=='0') break;
    randomDelay();
	randomDelay();
	}
    return 0;
}


DWORD sem_wait(HANDLE sem){
    DWORD result = WaitForSingleObject(sem,INFINITE);

	switch(result){
		case WAIT_OBJECT_0:return 1;
		case WAIT_TIMEOUT: return 0;
	}
	return 0;
}

DWORD sem_signal(HANDLE sem){
    return ReleaseSemaphore(sem,1,NULL); // Increase by one
}
