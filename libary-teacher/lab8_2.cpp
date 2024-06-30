#include <stdio.h>
#include <windows.h>

int a;
thread_local int b;
DWORD WINAPI threadFunction(LPVOID selector);

int main(void){
	DWORD tid[4];				// Thread ID
	HANDLE th[4];					// Thread Handle
	int section[4] = { 0,1,2,3 };

	b = -1;

	for (int i=0;i<4;i++)
		th[i] = CreateThread(NULL, 0, threadFunction, (void *)&section[i], 0, &tid[i]);

	for (int i=0;i<4;i++)
		WaitForSingleObject(th[i],INFINITE);
	return 0;
}

DWORD WINAPI threadFunction(LPVOID selector){
	int sel=*((int *)selector);

	b = sel;

	Sleep(1000);
	printf("b=%d\n", b);
	return 0;
}
