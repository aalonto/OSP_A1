#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SIZE 10
#define MAX_THREADS 5
#define RUNTIME 10
#define EMPTY 0

pthread_mutex_t array_lock = PTHREAD_MUTEX_INITIALIZER;

bool isFilled = false;
pthread_cond_t is_ready = PTHREAD_COND_INITIALIZER;

pthread_mutex_t finished_lock = PTHREAD_MUTEX_INITIALIZER;
bool finished = false;

int i;
int in = 0;
int out = 0;
int filled = 0;
time_t begin, current;

int buckets[MAX_SIZE];

void *producer(void *index)
{
    int item;
    current = time(NULL);

    while (true)
    {
        current = time(NULL);
        if (difftime(current, begin) >= RUNTIME)
        {
            printf("Producer %d: Started exiting loop\n", *((int *)index));
            break;
        }
        item = rand();
        pthread_mutex_lock(&array_lock);
        printf("Producer %d: Acquire mutex\n", *((int *)index));
        if (isFilled || filled == MAX_SIZE)
        {
            printf("Producer %d: Waiting for producer thread\n", *((int *)index));
            pthread_cond_wait(&is_ready, &array_lock);
        }
        if (buckets[in] == EMPTY)
        {
            printf("Producer %d: Insert Item %d To Bucket %d\n", *((int *)index), item, in);
            buckets[in] = item;
            in = (in + 1) % MAX_SIZE;
            filled++;
        }
        isFilled = true;

        printf("Producer %d: Unlock mutex\n", *((int *)index));
        pthread_mutex_unlock(&array_lock);
        pthread_cond_signal(&is_ready);
    }

    pthread_mutex_lock(&finished_lock);

    finished = true;

    printf("Producer %d: Set finished to true\n", *((int *)index));


    pthread_mutex_unlock(&finished_lock);

    pthread_exit(NULL);
}

void *consumer(void *index)
{
    while (true)
    {
        pthread_mutex_lock(&finished_lock);
        printf("Consumer %d: Lock finished mutex\n", *((int *)index));
        if (finished)
        {
            pthread_mutex_unlock(&finished_lock);
            printf("Consumer %d: Exiting loop\n", *((int *)index));

            printf("Consumer %d: Unlock finished mutex\n", *((int *)index));
            break;
        }
        pthread_mutex_unlock(&finished_lock);

        pthread_mutex_lock(&array_lock);
        printf("Consumer %d: Acquire mutex\n", *((int *)index));
        if (!isFilled || filled == EMPTY)
        {
            pthread_cond_wait(&is_ready, &array_lock);
        }

        if (buckets[out] != EMPTY)
        {
            int item = buckets[out];
            buckets[out] = EMPTY;
            filled--;
            printf("Consumer %d: Remove Item %d From Bucket %d\n", *((int *)index), item, out);
            out = (out + 1) % MAX_SIZE;
        }
        isFilled = false;
        printf("Consumer %d: Unlock mutex\n", *((int *)index));

        pthread_mutex_unlock(&array_lock);
        pthread_cond_signal(&is_ready);
    }

    pthread_exit(NULL);
}

int main()
{
    begin = time(NULL);

    pthread_t prod_threads[5], cons_threads[5];

    pthread_mutex_init(&array_lock, NULL);
    pthread_cond_init(&is_ready, NULL);

    int thread[MAX_THREADS] = {1, 2, 3, 4, 5};

    for (i = 0; i < MAX_SIZE; i++)
    {
        printf("Adding bucket %d\n", i);
        buckets[i] = EMPTY;
    }

    for (i = 0; i < MAX_THREADS; ++i)
    {
        pthread_create(&prod_threads[i], NULL, (void *)producer, (void *)&thread[i]);
        pthread_create(&cons_threads[i], NULL, (void *)consumer, (void *)&thread[i]);
    }

    for (i = 0; i < MAX_THREADS; ++i)
    {
        pthread_join(prod_threads[i], NULL);
        pthread_join(cons_threads[i], NULL);

    }

    pthread_mutex_destroy(&array_lock);
    printf("Program exited successfully\n");


    return EXIT_SUCCESS;
}