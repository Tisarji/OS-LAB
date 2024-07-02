#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

struct {
	int n1[BUFFER_SIZE];
	int n2[BUFFER_SIZE];
} data;

void input_data(int i)
{
	printf("\nProcess A: \n");
	printf("NO_idx.[%d] :Enter Data First: ", i);
	scanf("%d", &data.n1[i]);
	printf("NO_idx.[%d] :Enter Data Second: ", i);
	scanf("%d", &data.n2[i]);
}

void process_B(int index)
{
	printf("Process B: NO.[%d] First data: %d\n", index, data.n1[index]);
	sleep(3);
}

void process_C(int index)
{
	sleep(3);
	printf("Process C: NO.[%d] Second data: %d\n", index, data.n2[index]);
}

int main(void)
{
	pid_t pid1, pid2;
	int i = 0;

	while (1)
	{
		input_data(i);
		if (data.n1[i] < 0)
		{
			printf("Terminating program.\n");
			break;
		}

		pid1 = fork();
		if (pid1 < 0)
		{
			printf("Fork Failed!");
			exit(EXIT_FAILURE);
		}
		else if (pid1 == 0)
		{
			process_B(i);
			exit(EXIT_SUCCESS);
		}

		pid2 = fork();
		if (pid2 < 0)
		{
			printf("Fork Failed!");
			exit(EXIT_FAILURE);
		}
		else if (pid2 == 0)
		{
			process_C(i);
			exit(EXIT_SUCCESS);
		}

		i++;
	}
	// Wait for all child processes to complete
	while (wait(NULL) > 0);
	return (0);
}
