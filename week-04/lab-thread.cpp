#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>

#define BUFFER_SIZE 100

struct {
	int *n1;
	int *n2;
	int i = 0;
} data;

void	*process_A(void *arg)
{
	while (1)
	{
		printf("\033[0;36m\nProcess A: \n\033[0m");
		printf("\033[0;36mNO_idx.[%d] :Enter Data First: \033[0m", data.i);
		scanf("%d", &data.n1[data.i]);
		printf("\033[0;36mNO_idx.[%d] :Enter Data Second: \033[0m", data.i);
		scanf("%d", &data.n2[data.i]);
		data.i++;
	}
	pthread_exit(NULL);
}

void	*process_B(void *arg)
{
	sleep(3);
	printf("\033[0;32mProcess B: NO.[%d] First data: %d \033[0m\n", data.i, data.n1[data.i]);
	pthread_exit(NULL);
}

void	*process_C(void *arg)
{
	if (data.i != 0)
		sleep(3);
	printf("\033[0;32mProcess C: NO.[%d] Second data: %d \033[0m\n", data.i, data.n2[data.i]);
	pthread_exit(NULL);
}

int	main(void)
{
	pthread_t	thread[BUFFER_SIZE * 2];
	int index[BUFFER_SIZE];
	int	j = 0;

	data.n1 = (int *)malloc(BUFFER_SIZE * sizeof(int));
	data.n2 = (int *)malloc(BUFFER_SIZE * sizeof(int));
	while (1)
	{
		pthread_create(&thread[data.i], NULL, process_A, &index[data.i]);
		if (data.n1[data.i] < 0)
		{
			printf("Terminating programe.\n");
			break ;
		}
		index[data.i] = data.i;
		pthread_create(&thread[data.i], NULL, process_B, &index[data.i]);
		pthread_create(&thread[data.i], NULL, process_C, &index[data.i]);
		data.i++;
	}
	while (j < data.i)
		pthread_join(thread[j++], NULL);
	free(data.n1);
	free(data.n2);
	return (0);
}
