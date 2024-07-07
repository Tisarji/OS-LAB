#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

struct {
	int	n[BUFFER_SIZE];
	int	power[BUFFER_SIZE];
} data;

void input_data(int i)
{
	int sum = 0;

	printf("\033[0;36mProcess A: \n\033[0m");
	printf("\033[0;36mNO_idx.[%d] :Enter Data : \033[0m", i);
	scanf("%d", &sum);
	data.n[i] = sum;
	data.power[i] = sum * 2;
}

void process_B(int index)
{
	sleep(1);
	printf("\033[0;32mProcess B: NO.[%d] Data Shared: %d \033[0m\n", index, data.n[index]);
}

void process_C(int index)
{
	sleep(2);
	printf("\033[0;32mProcess C: NO.[%d] Data Power of 2: %d \033[0m\n", index, data.power[index]);
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
			break ;
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
			process_B(i);
			exit(EXIT_SUCCESS);
			waitpid(pid1, NULL, 0);
		}
		pid2 = fork();
		if (pid2 < 0)
		{
			printf("Fork failed!");
			exit(EXIT_FAILURE);
		}
		else if (pid2 == 0)
		{
			process_C(i);
			exit(EXIT_SUCCESS);
			waitpid(pid2, NULL, 0);
		}
		i++;
	}
	return (0);
}
