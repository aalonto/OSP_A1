#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

sem_t phil[MAX];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
time_t begin, current;
int phil_index[MAX] = {0, 1, 2, 3, 4};
int eat_count[MAX];
int phil_states[MAX];

void test_and_eat(int num)
{
	bool chopsticksFree = phil_states[LEFT] != EATING && phil_states[RIGHT] != EATING;
	if (phil_states[num] == HUNGRY && chopsticksFree)
	{
		phil_states[num] = EATING;

		printf("Philosopher %d is eating.\n", num + 1);
		sleep(RANDOM_TIME);
		eat_count[num] += 1;

		sem_post(&phil[num]);
		printf("Philosopher %d is thinking.\n", num + 1);

	}
}

void grab_chopsticks(int num)
{
	pthread_mutex_lock(&lock);

	printf("Philosopher %d is hungry.\n", num + 1);
	phil_states[num] = HUNGRY;
	test_and_eat(num);

	pthread_mutex_unlock(&lock);
	sem_wait(&phil[num]);
}

void put_down_chopsticks(int num)
{
	pthread_mutex_lock(&lock);

	printf("Philosopher %d has finished eating.\n", num + 1);

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

		printf("Philosopher %d is thinking.\n", *phil_num + 1);
		sleep(RANDOM_TIME);

		grab_chopsticks(*phil_num);
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
		sem_init(&phil[i], 0, 0);
		printf("Philosopher %d has entered the room.\n", i+1);
	}

	for(i = 0; i < MAX; ++i) {
		eat_count[i] = 0;
	}

	for (i = 0; i < MAX; ++i)
	{
		pthread_create(&philosophers[i], NULL, philosopher, &phil_index[i]);
	}
	for (i = 0; i < MAX; ++i)
	{
		pthread_join(philosophers[i], NULL);
	}

	for (i = 0; i < MAX; ++i)
	{
		printf("Philosopher %d has eaten %d times.\n", i+1, eat_count[i]);
	}

	printf("Program exited successfully.\n");

	return EXIT_SUCCESS;

}