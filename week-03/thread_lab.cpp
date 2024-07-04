#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 100

struct {
	int* n1;
	int* n2;
} data;

void	input_data(int i)
{
	printf("\033[0;36m\nProcess A: \n\033[0m");
	printf("\033[0;36mNO_idx.[%d] :Enter Data First: \033[0m", i);
	scanf("%d", &data.n1[i]);
	printf("\033[0;36mNO_idx.[%d] :Enter Data Second: \033[0m", i);
	scanf("%d", &data.n2[i]);
}

void	*process_B(void* arg)
{
	int index = *(int*)arg;
	sleep(3);
	printf("\033[0;32mProcess B: NO.[%d] First data: %d \033[0m\n", index, data.n1[index]);
	pthread_exit(NULL);
}

void	*process_C(void* arg)
{
	int index = *(int*)arg;
	if (index != 0)
		sleep(3);
	printf("\033[0;32mProcess C: NO.[%d] Second data: %d \033[0m\n", index, data.n2[index]);
	pthread_exit(NULL);
}

int	main(void)
{
	pthread_t threads[BUFFER_SIZE * 2];
	int indices[BUFFER_SIZE];
	int i = 0;
	int	j = 0;
	data.n1 = (int*)malloc(BUFFER_SIZE * sizeof(int));
	data.n2 = (int*)malloc(BUFFER_SIZE * sizeof(int));

	while (1)
	{
		input_data(i);
		if (data.n1[i] < 0)
		{
			printf("Terminating program.\n");
			break;
		}
		indices[i] = i;
		pthread_create(&threads[i * 2], NULL, process_B, &indices[i]);
		pthread_create(&threads[i * 2 + 1], NULL, process_C, &indices[i]);
		i++;
	}
	while (j < i * 2)
		pthread_join(threads[j++], NULL);
	free(data.n1);
	free(data.n2);
	return (0);
}
