#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h> // for isdigit

#define BUFFER_SIZE 100

struct {
	float n[BUFFER_SIZE];
	float power[BUFFER_SIZE];
	float sum[BUFFER_SIZE];
} data;

int	is_valid_number(const char* str)
{
	int i = 0;

	while (str[i] != '\0')
	{
		if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-')
			return (0);
		i++;
	}
	return (1);
}

void	input_data(int i)
{
	char	temp_str[BUFFER_SIZE];
	float	temp = 0;

	printf("Enter Input: ");
	scanf("%s", temp_str);

	if (is_valid_number(temp_str))
	{
		temp = atof(temp_str);
		data.n[i] = temp;
		data.power[i] = temp * 2;
		data.sum[i] = temp + data.power[i];
	}
	else 
	{
		printf("Invalid input. Please enter a valid number.\n");
		i--;
	}
}

int	main(void)
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
		if (data.n[i] > 100)
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
			j = 0;
			while (j <= i)
			{
				printf("Data[%d] Power of 2: %f\n", j, data.power[j]);
				sleep(1);
				j++;
			}
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
			j = 0;
			while (j <= i)
			{
				printf("Data[%d] Sum: %f\n", j, data.sum[j]);
				sleep(2);
				j++;
			}
			exit(EXIT_SUCCESS);
		}
		waitpid(pid1, NULL, 0);
		waitpid(pid2, NULL, 0);
		i++;
	}
	return (0);
}
