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
int  lock;
int  waiting[4];

void randomDelay(void);
void serve(char *dest,char *src);
DWORD WINAPI chefWork(LPVOID who);

int main(void){
	int i;
	DWORD tid[5];				// Thread ID
	HANDLE th[5];				// Thread Handle
	int param[5]={0,1,2,3,4};

	for(i=0;i<4;i++)
		waiting[i]=false;
	isFull=20;
	lock = false;

	// Create 5 threads
	for(i=0;i<4;i++)
	th[i] = CreateThread(
		NULL,				// Default security attributes
		0,					// Default stack size
		chefWork,			// Thread function
		(void *)&param[i],	// Thread function parameter
		0,					// Default creation flag
		&tid[i]);			// Thread ID returned.


       // Wait until all threads finish
	for(i=0;i<4;i++)
		if(th[i]!=NULL)
			WaitForSingleObject(th[i],INFINITE);
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
	int key;

    chefNo = (int)*((int *)who);

    for(i=0;i<(chefNo+2);i++) printf("        ");
    printf("Chef NO %d start working...\n",chefNo+1);
    fflush(stdout);	

    while(isFull>0){
	waiting[chefNo]=true;
	key = true;
	while(waiting[chefNo] && key){
		key = InterlockedExchangeAdd((volatile LONG *)&lock,1);
		Sleep(500);
		if(isFull<=0) break;
	}
	waiting[chefNo]=false;
	if(isFull>0){
	// Critical Section
	     	for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d is serving the food\n",chefNo+1);
		fflush(stdout);	

		for(j=0;j<3;j++)
		    serve(plate[j],chef[chefNo][j]);

     		for(i=0;i<(chefNo+2);i++) printf("      ");
		printf("Chef NO %d has serve the food\n",chefNo+1);
		fflush(stdout);	
		isFull--;
	}
	j = (chefNo+1)%4;
	while((j!=chefNo)&& !waiting[j])
		j = (j+1)%4;
	if(j==chefNo){
		lock = false;
		printf("Chef No %d has unlocked the key.[%d][%d][%d][%d]\n",j+1,waiting[0],waiting[1],waiting[2],waiting[3]);
	}
	else{
		waiting[j]=false;
		printf("Passing to chef No %d [%d][%d][%d][%d]\n",j+1,waiting[0],waiting[1],waiting[2],waiting[3]);
	}
	// Remaining Section
	Sleep((rand()%8000)+500);
    }
    return 0;
}