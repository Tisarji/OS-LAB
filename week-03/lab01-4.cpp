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
	printf("\033[0;36m\nProcess A: \n\033[0m");
	printf("\033[0;36mNO_idx.[%d] :Enter Data First: \033[0m", i);
	scanf("%d", &data.n1[i]);
	printf("\033[0;36mNO_idx.[%d] :Enter Data Second: \033[0m", i);
	scanf("%d", &data.n2[i]);
}

void process_B(int index)
{
	sleep(3);
	printf("\033[0;32mProcess B: NO.[%d] First data: %d \033[0m\n", index, data.n1[index]);
}

void process_C(int index)
{
	if (index != 0)
		sleep(3);
	printf("\033[0;32mProcess C: NO.[%d] Second data: %d \033[0m\n", index, data.n2[index]);
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
	return (0);
}
