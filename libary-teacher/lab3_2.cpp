#include <stdio.h>
#include <windows.h>

int c[4][4];
int a[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
int b[4][4]={{10,-10,10,-10},{-10,10,-10,10},{10,-10,10,-10},{-10,10,-10,10}};

void showresult();
DWORD WINAPI threadFunction(LPVOID selector);

int main(void){
	DWORD tid1,tid2;				// Thread ID
	HANDLE th1,th2;					// Thread Handle
	int section1=0,section2=1;

	// Create 2 threads
	th1 = CreateThread(
		NULL,						// Default security attributes
		0,							// Default stack size
		threadFunction,				// Thread function
		(void *)&section1,			// Thread function parameter
		0,							// Default creation flag
		&tid1);						// Thread ID returned.

	th2 = CreateThread(
		NULL,						// Default security attributes
		0,							// Default stack size
		threadFunction,				// Thread function
		(void *)&section2,			// Thread function parameter
		0,							// Default creation flag
		&tid2);						// Thread ID returned.

       // Wait until all threads finish
	if((th1 && th2) != NULL){
		WaitForSingleObject(th1,INFINITE);
		WaitForSingleObject(th2,INFINITE);

		showresult();
	}
	return 0;
}

void showresult(){
	int i,j;

	for(j=0;j<4;j++){
		printf("|");
		for(i=0;i<4;i++){
			printf("%3d ",c[j][i]);
		}
		printf("|\n");
	}
}
DWORD WINAPI threadFunction(LPVOID selector){
	int sel=*((int *)selector);
	int start,stop;
	int i,j;

	start=sel*2;
	stop =start+2;

	for(j=start;j<stop;j++)
		for(i=0;i<4;i++){
			c[j][i]=a[j][i]+b[j][i];
			// To Show how thread runs, this will slow thing down...
			Sleep(1000);
			printf("From thread%d i=%d j=%d\n",sel,i,j);
			fflush(stdout); // Send text to display immediately
		}
	return 0;
}
