#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

struct {
	char	str[BUFFER_SIZE];
	int		length;
} data;

void input_data(int i)
{
	char temp_str[BUFFER_SIZE];

	printf("Enter Input: ");
	scanf("%s", temp_str);
	strcpy(data.str, temp_str);
	data.length = strlen(data.str);
}

int main(void)
{
	pid_t	pid1;
	pid_t	pid2;
	int		i;
	int		j;

	i = 0;
	j = 0;
	pid1 = 0;
	pid2 = 0;
	while (1)
	{
		input_data(i);
		if (data.str[0] == '0')
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
			printf("Data String: %s\n", data.str);
			sleep(3);
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
			printf("Data Length: %d\n", data.length);
			sleep(3);
			exit(EXIT_SUCCESS);
		}
		waitpid(pid1, NULL, 0);
		waitpid(pid2, NULL, 0);
		i++;
	}
	return (0);
}
