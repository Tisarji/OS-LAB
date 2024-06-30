#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include "winconio.h"


// Try using one of these options to see how to recover deadlock
//#define _LOCK
//#define _TRYLOCK
#define _TIMEDLOCK

char plate[3][64]; // Consumer table
char chef[4][3][64] = {
	{"rice with ","chicken curry, and ","fish sauce."},
	{"wiskey with ","lemonade, and ","soda."},
	{"bread with ","cheese, and ","ketchup."},
	{"icecream with ","banana, and ","chocolate."}};
int  isFull;
int  isReady;

HANDLE mutex,display;

void randomDelay(void);
void randomDelay2(void);
void serve(char *dest,char *src);
DWORD WINAPI chefWork(LPVOID who);
DWORD WINAPI customer(LPVOID who);

int main(void){
	int i;
	DWORD tid[5];				// Thread ID
	HANDLE th[5];				// Thread Handle
	int param[5]={0,1,2,3,4};

	clrscr();

	mutex = CreateMutex(NULL,false,NULL);
	display = CreateMutex(NULL,false,NULL);

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
	CloseHandle(mutex);
	CloseHandle(display);
	return 0;
}

void randomDelay(void){
    int stime = ((rand()%10)+1);
    Sleep(stime);
}

void randomDelay2(void){
    Sleep(1);	// Try comment this line to reduce the time between requesting two resources
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
	DWORD dwWaitResult;

    chefNo = (int)*((int *)who);

	WaitForSingleObject(display,INFINITE);
	gotoxy(1,chefNo*3+3);
	printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);
	ReleaseMutex(display);

    while(!isFull){

	if(isReady) {
           randomDelay(); // Wait for food to be taken
	}

	WaitForSingleObject(mutex,INFINITE);
	randomDelay2();

#if defined(_TRYLOCK)
	if(WaitForSingleObject(display,0)==WAIT_TIMEOUT){	// Try if the lock is a success
		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d Cannot lock the display     \n",chefNo+1);
		fflush(stdout);
		ReleaseMutex(mutex);	// Release the resource
		continue;
	}
#elif defined(_TIMEDLOCK)
	if(WaitForSingleObject(display,2000L)==WAIT_TIMEOUT){	// wait 2 seconds
		gotoxy(1,chefNo*3+3);
		printf("Chef NO %d time out...        \n",chefNo+1);
		fflush(stdout);
		ReleaseMutex(mutex);	// Release the resource
		continue;
	}
#else
	WaitForSingleObject(display,INFINITE);
#endif
		// Critical Section
		if((!isFull)&&(!isReady)){
	     	gotoxy(1,chefNo*3+3);
			printf("Chef NO %d is serving the food                   \n",chefNo+1);
			fflush(stdout);

			for(j=0;j<3;j++)
			serve(plate[j],chef[chefNo][j]);
			isReady=true; //Tel customer that the food is ready

     		gotoxy(1,chefNo*3+3);
			printf("Chef NO %d has serve the food                    \n",chefNo+1);
			fflush(stdout);
		}
	ReleaseMutex(display);
	ReleaseMutex(mutex);
	// Remaining Section

	if(isFull) break;
	randomDelay();  // This will cause some chef to waiting indefinitly;
    }
    return 0;
}

DWORD WINAPI customer(LPVOID who){
    int i,j;
    char dinner[256]={0};

    for(i=0;i<10;i++){

	while(!isReady) randomDelay();

	WaitForSingleObject(display,INFINITE);
	randomDelay2();
	WaitForSingleObject(mutex,INFINITE);
	// Critical Section
		gotoxy(1,1);
		printf("Choochok starts grab a set of dinner                                            \n");
		fflush(stdout);

		serve(dinner,plate[0]); dinner[63]=0;
		serve(dinner + strlen(dinner),plate[1]); dinner[127]=0;
		serve(dinner + strlen(dinner),plate[2]); dinner[191]=0;

		gotoxy(1,1);
		printf("Plate NO:%d Choochok is eating %s                                            \n",
			i+1,dinner);
		fflush(stdout);

		isReady=false;	// Food taken

	ReleaseMutex(mutex);
	ReleaseMutex(display);
	// Remaining Section
	}
	isFull = true; // Tell other producer threads to stop;
    return 0;
}
