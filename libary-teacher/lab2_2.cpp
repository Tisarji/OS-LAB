#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <process.h>

int main(int argc,char *argv[],char **env){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR name[] = _T("lab2_2.exe 1");
//	TCHAR name[] = _T("lab00_1.exe 1");

	if(argc<2){
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));

    printf("Before fork a child process...\n");

    if(!CreateProcess(NULL,name,
		NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
			fprintf(stderr,"Create process failed.\n");
			return -1;
	}

	//Parent Process
	printf("Before going into the wait state...\n");
	WaitForSingleObject(pi.hProcess,INFINITE);
	printf("Child process has terminated\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	}else{
	    // Child process
	    printf("Child process is running...");
        _execlp("c:\\windows\\system32\\notepad.exe","notepad",NULL);
//	    Sleep(10000);
	}
	return 0;
}
