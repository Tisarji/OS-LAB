#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 50
//g++ <name> -lpthread

struct {
	int	n1[BUFFER_SIZE];
	int	n2[BUFFER_SIZE];
	int	count = 0;
}num;

void showresult();
void *threadFunction1(void *selector);//parameter must be void*0
void *threadFunction2(void *selector);
void *threadFunction3(void *selector);

int main(void)
{
	pthread_t tid1,tid2,tid3;		// Thread ID
	pthread_attr_t attr1,attr2,attr3;  	// Thread attributes

	

	pthread_attr_init(&attr1);  // Get default attributes
	pthread_attr_init(&attr2);  // Get default attributes
	pthread_attr_init(&attr3);

	// Create 2 threads, share funcetion together.
	pthread_create(&tid1,&attr1,threadFunction1,NULL);
	pthread_create(&tid2,&attr2,threadFunction2,NULL);
	pthread_create(&tid3,&attr3,threadFunction3,NULL);

		 // Wait until all threads finish
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);

	//pthread_join(tid1,(void **)&recint1);
	//printf("%d", *recint1);
	return 0;
}

void	*threadFunction1(void *selector)
{
	int count;

	count = 0;
	int num1_local = 0;
	int	num2_local = 0;
	while(1)
	{
		printf("Enter first number no.%d  : ", count + 1);
		scanf("%d",&num1_local);
		printf("Enter second number no.%d : ", count + 1);
		scanf("%d",&num2_local);
		if (num.n1[count] < 0)
		{
			printf("Terminating Program.\n");
			pthread_exit(0);
		}
		num.count = num.count + 1;
		count++;
	}
}
void	*threadFunction2(void *selector)
{
	int count;

	count =  0;
	while(1)
	{
		if (count < num.count)
		{
			if (num.n1[count] < 0)
			{
				printf("Terminating Program.\n");
				pthread_exit(0);
			}
			sleep(3);
			printf("NO.%d First number  : %d\n", count + 1, num.n1[count]);
			count++;	
		}
		
	}
}
void *threadFunction3(void *selector)
{
	int count;

	count = 0;
	while(1)
	{
		if (count < num.count)
		{
			if (num.n1[count] < 0)
			{
				printf("Terminating Program.\n");
				pthread_exit(0);
			}
			printf("NO.%d Second number : %d\n", count + 1, num.n2[count]);
			sleep(3);
			count++;	
		}
	}
}
