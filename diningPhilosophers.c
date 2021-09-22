#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define RUNTIME 10
#define RANDOM_TIME (float)rand() / RAND_MAX
#define MAX 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define LEFT (num + 4) % MAX
#define RIGHT (num + 1) % MAX

sem_t chopstick[MAX];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
time_t begin, current;
int phil[MAX] = {0, 1, 2, 3, 4};
int phil_states[MAX];

void test_and_eat(int num)
{
	if (phil_states[num] == HUNGRY && phil_states[LEFT] != EATING && phil_states[RIGHT] != EATING)
	{
		phil_states[num] = EATING;

		printf("Philosopher %d takes fork %d and %d.\n",
			   num + 1, LEFT + 1, num + 1);

		printf("Philosopher %d is eating.\n", num + 1);

		sem_post(&chopstick[num]);
	}
}

void grab_chopsticks(int num)
{
	pthread_mutex_lock(&lock);

	printf("Philosopher %d is hungry.\n", num);
	phil_states[num] = HUNGRY;
	test_and_eat(num);

	pthread_mutex_unlock(&lock);
	sem_wait(&chopstick[num]);
}

void put_down_chopsticks(int num)
{
	pthread_mutex_lock(&lock);

	printf("Philosopher %d is putting fork %d and %d down.\n",
		   num + 1, LEFT + 1, RIGHT + 1);

	printf("Philosopher %d is thinking.\n", num + 1);
	phil_states[num] = THINKING;

	test_and_eat(LEFT);
	test_and_eat(RIGHT);

	pthread_mutex_unlock(&lock);
}

void *philosopher(void *num)
{
	do
	{
		current = time(NULL);
		int* phil_num = num;

		printf("Philosopher %d is thinking.\n", *phil_num);
		sleep(RANDOM_TIME);

		grab_chopsticks(*phil_num);
		printf("Philosopher %d has finished eating. \n", *phil_num);
		sleep(RANDOM_TIME);

		put_down_chopsticks(*phil_num);

	} while (difftime(current, begin) <= RUNTIME);
}

int main()
{
	srand((unsigned)time(NULL));
	begin = time(NULL);
	int i;
	pthread_t philosophers[5];

	pthread_mutex_init(&lock, NULL);

	for (i = 0; i < MAX; ++i)
	{
		sem_init(&chopstick[i], 0, 0);
	}
	for (i = 0; i < MAX; ++i)
	{
		pthread_create(&philosophers[i], NULL, philosopher, &phil[i]);
	}
	for (i = 0; i < MAX; ++i)
	{
		pthread_join(philosophers[i], NULL);
	}

}