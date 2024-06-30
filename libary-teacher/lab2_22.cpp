#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <process.h>

struct shmareatype{
   int x;
};

void processA(struct shmareatype *shmarea);
void processB(struct shmareatype *shmarea);
void processC(struct shmareatype *shmarea);

int main(int argc,char *argv[],char **env){
	STARTUPINFO si,si2;
	PROCESS_INFORMATION pi,pi2;
	TCHAR name[] = _T("lab2_22.exe 1");
	TCHAR name2[] = _T("lab2_22.exe 1 1");
    TCHAR shmName[] = _T("lab2_4");

    struct shmareatype *shmarea; // Pointer to the shared segment
    int shmsize = sizeof(struct shmareatype); // share segment size
	HANDLE hMapFile;

	if(argc<2){
    hMapFile = CreateFileMapping(
				 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // max. object size
                 shmsize,                // buffer size
                 shmName);                 // name of mapping object

	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE){
      printf("Could not create file mapping object...\n");
      return -1;
    }
    shmarea = (struct shmareatype *) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        shmsize);

    if (shmarea == NULL)
    {
      printf("Could not map view of file (%d).\n",
             GetLastError());
      return -1;
    }
    shmarea->x=5;
	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	ZeroMemory(&si2,sizeof(si2));
	si2.cb = sizeof(si2);
	ZeroMemory(&pi2,sizeof(pi2));

    printf("Before fork a child process...\n");

    if(!CreateProcess(NULL,name,
		NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
			fprintf(stderr,"Create process failed.\n");
			return -1;
	}
    if(!CreateProcess(NULL,name2,
		NULL,NULL,FALSE,0,NULL,NULL,&si2,&pi2)){
			fprintf(stderr,"Create process failed.\n");
			return -1;
	}
	//Parent Process
	processA(shmarea);
	WaitForSingleObject(pi.hProcess,INFINITE);
	WaitForSingleObject(pi2.hProcess,INFINITE);
	printf("Child process has terminated\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(pi2.hProcess);
	CloseHandle(pi2.hThread);

    UnmapViewOfFile(shmarea);
    CloseHandle(hMapFile);
	}else if(argc==2){
	    // Child process
        hMapFile = OpenFileMapping(
				   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   shmName);               // name of mapping object

     if (hMapFile == NULL) {
      printf("Could not open file mapping object (%d).\n",
             GetLastError());
      return -1;
     }
    shmarea = (struct shmareatype *) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        shmsize);

    if (shmarea == NULL)
    {
      printf("Could not map view of file (%d).\n",
             GetLastError());
      return -1;
    }
        processB(shmarea);
       UnmapViewOfFile(shmarea);
       CloseHandle(hMapFile);
        return 0;
	}else{
        hMapFile = OpenFileMapping(
				   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   shmName);               // name of mapping object

     if (hMapFile == NULL) {
      printf("Could not open file mapping object (%d).\n",
             GetLastError());
      return -1;
     }
     shmarea = (struct shmareatype *) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        shmsize);

    if (shmarea == NULL)
    {
      printf("Could not map view of file (%d).\n",
             GetLastError());
      return -1;
    }
        processC(shmarea);
        	    UnmapViewOfFile(shmarea);
       CloseHandle(hMapFile);
        return 0;
	}
	return 0;
}

void processA(struct shmareatype *shmarea){
    printf("From process A\n");
    printf("x=%d\n",shmarea->x);
}

void processB(struct shmareatype *shmarea){
    printf("From process B\n");
    printf("x=%d\n",shmarea->x);
}

void processC(struct shmareatype *shmarea){
    printf("From process C\n");
    printf("x=%d\n",shmarea->x);
}
