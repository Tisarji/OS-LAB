#include <windows.h>
#include <stdio.h>
#include <tchar.h>

struct shmareatype{
   int rp;
   int wp;
   char data[256];
};

void consumer(struct shmareatype *shmarea);
void producer(struct shmareatype *shmarea);

int main(int argc, char *argv[]){
	TCHAR shmName[] = _T("lab2_4");
	struct shmareatype *shmarea; // Pointer to the shared segment
    int shmsize = sizeof(struct shmareatype); // share segment size
	HANDLE hMapFile;

	if(argc>1){ // If this is the child process it will run this instead
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

        consumer(shmarea);

	    UnmapViewOfFile(shmarea);
       CloseHandle(hMapFile);
		return 0;
	}
    // Create Memory Mapping File
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

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR name[] = _T("lab2_4.exe 1"); // Using the Same code with parameter
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
	// Map share memory
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
	shmarea->rp = shmarea->wp = 0;
	producer(shmarea);
	printf("Before going into the wait state...\n");
	WaitForSingleObject(pi.hProcess,INFINITE);
	printf("Child process has terminated\n");
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	UnmapViewOfFile(shmarea);
    CloseHandle(hMapFile);
    return 0;
}

void consumer(struct shmareatype *shmarea){
    int i;

    for(i=0;i<16;i++) // consume data for 16 times
    {
       // Wait 2 minutes if no data in the circular buffer
       while(shmarea->wp==shmarea->rp) Sleep(2000);
       printf("Data number:%d = %c\n",i,shmarea->data[shmarea->rp]);
       shmarea->rp = ((shmarea->rp+1)%256);
    }
}

void producer(struct shmareatype *shmarea){
    int i;
    char temp[16];

    for(i=0;i<16;i++) // produce data for 16 times
    {
	printf("Please enter a character No %d :",i);
        fgets(temp,16,stdin);
        printf("Enter data %c into share memory...\n",temp[0]);
         shmarea->data[shmarea->wp]=temp[0];
       // move the write pointer so that the consumer know when to read.
       shmarea->wp = ((shmarea->wp+1)%256);
    }
}
