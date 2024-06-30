#include <stdio.h>
#include <windows.h>

int x;

DWORD WINAPI threadFunctionA(LPVOID selector);
DWORD WINAPI threadFunctionB(LPVOID selector);

int main(void){
	DWORD tid1,tid2;				// Thread ID
	HANDLE th1,th2;					// Thread Handle

    x =5;
	// Create 2 threads
	th1 = CreateThread(
		NULL,						// Default security attributes
		0,							// Default stack size
		threadFunctionA,				// Thread function
		NULL,			// Thread function parameter
		0,							// Default creation flag
		&tid1);						// Thread ID returned.

	th2 = CreateThread(
		NULL,						// Default security attributes
		0,							// Default stack size
		threadFunctionB,				// Thread function
		NULL,			// Thread function parameter
		0,							// Default creation flag
		&tid2);						// Thread ID returned.

       // Wait until all threads finish
	if((th1 && th2) != NULL){
		WaitForSingleObject(th1,INFINITE);
		WaitForSingleObject(th2,INFINITE);
	}
	return 0;
}

DWORD WINAPI threadFunctionA(LPVOID selector){
    printf("From thread A\n");
    printf("x=%d\n",x);
    Sleep(5000);
	return 0;
}

DWORD WINAPI threadFunctionB(LPVOID selector){
    printf("From thread B\n");
    printf("x=%d\n",x);
    Sleep(5000);
	return 0;
}
