#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> // for usleep()
#include <stdlib.h> // for exit() and random generator
#include <wait.h>   // for wait()
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <time.h>

#define BUF_SIZE 256

struct shmareatype {
	int rp;
	int wp;
	int data[BUF_SIZE];
	int counter;
	// Following section is for Peterson's solution
	int flag[2];
	int turn;
};

void randomDelay(void);
void consumer(struct shmareatype* shmarea);
void producer(struct shmareatype* shmarea);
int increment(int* counter);
int decrement(int* counter);

int main()
{
	int shmid;  // Share memory ID
	struct shmareatype* shmarea; // Pointer to the shared segment
	int shmsize; // share segment size
	pid_t  pid;   // Child Process ID


	shmsize = sizeof(struct shmareatype);
	// Allocate the shared block from OS
	shmid = shmget(IPC_PRIVATE, shmsize, 0666 | IPC_CREAT);

	// Attach the shared block to the pointer so we can use it.
	shmarea = (struct shmareatype*)shmat(shmid, NULL, 0);

	shmarea->rp = shmarea->wp = shmarea->counter = 0;

	// Peterson's solution
	shmarea->flag[0] = shmarea->flag[1] = 0;
	shmarea->turn = 0;

	pid = fork();  //Fork a child process

	if (pid < 0) { //Fork error
		fprintf(stderr, "Fork failed.\n");
		exit(-1);
	}
	else if (pid == 0) { // This is the path of child process
		consumer(shmarea);
	}
	else { // This is the path of parent process
		producer(shmarea);
		wait(NULL);
		printf("Child process has terminated\n");

		// Detach the shared memory segment
		shmdt(shmarea);
		// Remove the shared memory segment
		shmctl(shmid, IPC_RMID, NULL);
		exit(0);
	}
}

void consumer(struct shmareatype* shmarea)
{
	int i = 0;
	int remaintime;

	while (1) // consume data
	{
		printf("       (Consumer) Before entering critical section\n");
		// Peterson's   ENTRY section
		shmarea->flag[0] = true;
		shmarea->turn = 1;
		while (shmarea->flag[1] && shmarea->turn == 1) usleep(1000);
		/////////////////////////////
		// Critical Section
		remaintime = 5000; // Wait some seconds before quitting
		while ((shmarea->counter == 0) && (remaintime > 0)) {
			usleep(1000);remaintime--;
			if (!(remaintime % 1000))
				printf("        Waiting for %d second(s).\n", remaintime / 1000);
		}
		if (remaintime == 0) {
			printf("        The consumer process waiting too long, stopping...\n");
			shmarea->flag[0] = false; // EXIT here in case of waiting too long
			return;
		}
		else {
			printf("        (Consumer) Data number:%d = %d\n", i++, shmarea->data[shmarea->rp]);
			shmarea->rp = ((shmarea->rp + 1) % BUF_SIZE);
			decrement(&(shmarea->counter));
		}
		// Peterson's 	EXIT section
		shmarea->flag[0] = false;
		// Remaining Section
		printf("       (Consumer) After exiting critical section\n");
	}
}

void producer(struct shmareatype* shmarea)
{
	int i;

	for (i = 0;i < 32;i++) // produce data
	{
		printf("(Producer) Before entering critical section\n");
		// Peterson's ENTRY section
		shmarea->flag[1] = true;
		shmarea->turn = 0;
		while (shmarea->flag[0] && shmarea->turn == 0) usleep(1000);
		// Critical Section
		while (shmarea->counter == BUF_SIZE) usleep(1000);
		printf("(Producer) Enter data %d into share memory...\n", i);
		shmarea->data[shmarea->wp] = i;
		// move the write pointer so that the consumer know when to read.
		shmarea->wp = ((shmarea->wp + 1) % BUF_SIZE);
		increment(&(shmarea->counter));
		// Peterson's  EXIT section
		shmarea->flag[1] = false;
		// Remaining Section
		printf("(Producer) After exiting critical section\n");
	}
}

void randomDelay(void)
{
	// This function provides a delay which slows the process down so we can see what happens
	srand(time(NULL));
	int stime = ((rand() % 1000) + 100) * 1000;
	usleep(stime);
}

int increment(int* counter)
{
	// This function show how non-atomic instruction can cause problem in consumer-producer case
	int temp = *counter;printf("INC: read\n");
	randomDelay();
	temp = temp + 1;printf("INC: ++\n");
	randomDelay();
	*counter = temp;printf("INC: write\n");
	return temp;
}

int decrement(int* counter)
{
	// This function show how non-atomic instruction can cause problem in consumer-producer case
	int temp = *counter;printf("       DEC: read\n");
	randomDelay();
	temp = temp - 1;printf("       DEC: --\n");
	randomDelay();
	*counter = temp;printf("       DEC: write\n");
	return temp;
}
