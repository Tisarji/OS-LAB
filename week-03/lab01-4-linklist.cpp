#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

struct {
	int *n1;
	int *n2;
} data;

void	input_data(int i)
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

	data.n1 = (int *)malloc(sizeof(int));
	data.n2 = (int *)malloc(sizeof(int));

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
			waitpid(pid1, NULL, 0);
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
			waitpid(pid2, NULL, 0);
			exit(EXIT_SUCCESS);
		}
		i++;
	}
	free(data.n1);
	free(data.n2);
	return (0);
}
