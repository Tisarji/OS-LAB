#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>


char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;
int  isReady;

HANDLE sem;

DWORD sem_wait(HANDLE sem);
DWORD sem_signal(HANDLE sem);

void randomDelay(void);
void serve(char *dest,char *src);
DWORD WINAPI chefWork(LPVOID who);
DWORD WINAPI customer(LPVOID who);

int main(void){
	int i;
	DWORD tid[5];				// Thread ID
	HANDLE th[5];				// Thread Handle
	int param[5]={0,1,2,3,4};

	sem = CreateSemaphore(
		NULL,   // default security attributes
		1,   // initial count
		1,   // maximum count
		NULL);  // unnamed semaphore

	// Create 5 threads
	for(i=0;i<4;i++)
	th[i] = CreateThread(
		NULL,				// Default security attributes
		0,					// Default stack size
		chefWork,			// Thread function
		(void *)&param[i],	// Thread function parameter
		0,					// Default creation flag
		&tid[i]);			// Thread ID returned.

	th[4] = CreateThread(
		NULL,				// Default security attributes
		0,					// Default stack size
		customer,			// Thread function
		(void *)&param[4],	// Thread function parameter
		0,					// Default creation flag
		&tid[4]);				// Thread ID returned.

       // Wait until all threads finish
	for(i=0;i<5;i++)
		if(th[i]!=NULL)
			WaitForSingleObject(th[i],INFINITE);
	CloseHandle(sem);
	return 0;
}

void randomDelay(void){
    int stime = ((rand()%100)+1);
    Sleep(stime);
}

void serve(char *dest,char *src){
   int i;
   srand(time(NULL));

   for(i=0;(i<63)&&(src[i]!=0);i++){
	dest[i]=src[i];
	randomDelay();
   }
   dest[i]=0;
}


DWORD WINAPI chefWork(LPVOID who){
    int chefNo,i,j;

    chefNo = (int)*((int *)who);

    for(i=0;i<(chefNo+2);i++) printf("        ");
    printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);

    while(!isFull){

	if(isReady) {
           randomDelay(); // Wait for food to be taken
	}

	sem_wait(sem);
	// Critical Section
		if((!isFull)&&(!isReady)){
	     	for(i=0;i<(chefNo+2);i++) printf("      ");
			printf("Chef NO %d is serving the food\n",chefNo+1);
			fflush(stdout);

			for(j=0;j<3;j++)
			serve(plate[j],chef[chefNo][j]);
			isReady=true; //Tel customer that the food is ready

     		for(i=0;i<(chefNo+2);i++) printf("      ");
			printf("Chef NO %d has serve the food\n",chefNo+1);
			fflush(stdout);
		}
	sem_signal(sem);
	// Remaining Section

	if(isFull) break;
	randomDelay();  // This will cause some chef to waiting indefinitly;
    }
    return 0;
}

DWORD WINAPI customer(LPVOID who){
    int i;
    char dinner[256]={0};

    for(i=0;i<10;i++){
//	printf("Choochok waits for food to be served\n");
//	fflush(stdout);

	while(!isReady) randomDelay();

	sem_wait(sem);
	// Critical Section

		printf("Choochok starts grab a set of dinner\n");
		fflush(stdout);

		serve(dinner,plate[0]); dinner[63]=0;
		serve(dinner + strlen(dinner),plate[1]); dinner[127]=0;
		serve(dinner + strlen(dinner),plate[2]); dinner[191]=0;

		printf("Plate NO:%d Choochok is eating %s\n",i+1,dinner);
		fflush(stdout);

		isReady=false;	// Food taken

	sem_signal(sem);
	// Remaining Section
	}
	isFull = true; // Tell other producer threads to stop;
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
