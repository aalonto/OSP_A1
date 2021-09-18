#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include <time.h>

#define RUNTIME 10
#define EATING_TIME 1
#define THINKING_TIME 3
#define MAX 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t room;
sem_t forks[MAX];
time_t begin, current;
int phil_index[MAX] = { 0, 1, 2, 3, 4 };

void * philosopher(void *);
void eat(int);
int main()
{
    begin = time(NULL);
	int i;
	pthread_t tid[5];
	
	sem_init(&room,0,4);
	
	for(i=0;i<MAX;i++) {
		sem_init(&forks[i],0,1);
	}
	for(i=0;i<MAX;i++){
		pthread_create(&tid[i],NULL,philosopher,&phil_index[i]);
	}
	for(i=0;i<MAX;i++) {
		pthread_join(tid[i],NULL);
    }
}

void * philosopher(void * num)
{
    do {
        current = time(NULL);
        int phil=*(int *)num;


    	sem_wait(&room);
    	printf("\nPhilosopher %d has entered room",phil);
    	sem_wait(&forks[phil]);
    	sem_wait(&forks[(phil+1)%5]);

    	eat(phil);
    	sleep(2);
    	printf("\nPhilosopher %d has finished eating",phil);

    	sem_post(&forks[(phil+1)%5]);
    	sem_post(&forks[phil]);
    	sem_post(&room);


    } while(difftime(current, begin) >= RUNTIME);
}

void eat(int phil)
{
	printf("\nPhilosopher %d is eating",phil);
}