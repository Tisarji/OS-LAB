#include <stdio.h>
#include <windows.h>
#include <tchar.h>

void main(){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR name[] = _T("c:\\windows\\system32\\notepad.exe");

	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	
    printf("Before fork a child process...\n");

    if(!CreateProcess(NULL,name,
		NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
			fprintf(stderr,"Create process failed.\n");
			return ;
	}

	//Parent Process
	printf("Before going into the wait state...\n");
	WaitForSingleObject(pi.hProcess,INFINITE);
	printf("Child process has terminated\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
