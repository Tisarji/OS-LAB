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
	printf("Enter Input First: ");
	scanf("%d", &data.n1[i]);
	printf("Enter Input Second: ");
	scanf("%d", &data.n2[i]);
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
			printf("First data = %d\n", data.n1[i]);
			sleep(3);
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
			printf("Second data = %d\n", data.n2[i]);
			sleep(3);
			exit(EXIT_SUCCESS);
		}
		waitpid(pid1, NULL, 0);
		waitpid(pid2, NULL, 0);
		i++;
	}
	return (0);
}
