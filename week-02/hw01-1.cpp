#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

struct {
	int n[BUFFER_SIZE];
	int power[BUFFER_SIZE];
} data;

void input_data(int i)
{
	int sum = 0;

	printf("Enter Input: ");
	scanf("%d", &sum);
	data.n[i] = sum;
	data.power[i] = sum * 2;
}

int main(void)
{
	pid_t pid1, pid2;
	int i = 0;

	while (1)
	{
		input_data(i);
		if (data.n[i] < 0)
		{
			printf("Terminating program.\n");
			exit(EXIT_FAILURE);
		}
		pid1 = fork();
		if (pid1 < 0)
		{
			printf("Fork failed!");
			exit(EXIT_FAILURE);
		}
		else if (pid1 == 0)
		{
			printf("Data Shared: %d\n", data.n[i]);
			sleep(1);
			exit(EXIT_SUCCESS);
		}
		pid2 = fork();
		if (pid2 < 0)
		{
			printf("Fork failed!");
			exit(EXIT_FAILURE);
		}
		else if (pid2 == 0)
		{
			printf("Data Power of 2: %d\n", data.power[i]);
			sleep(2);
			exit(EXIT_SUCCESS);
		}
		waitpid(pid1, NULL, 0);
		waitpid(pid2, NULL, 0);
		i++;
	}
	return (0);
}
