#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

const int MAX_SIZE = 10;
const int MAX_THREADS = 5;
const int RUNTIME = 10;

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

bool isFilled = false;
pthread_cond_t buffer_queue = PTHREAD_COND_INITIALIZER;

pthread_mutex_t finished_lock = PTHREAD_MUTEX_INITIALIZER;
bool finished = false;

int i;
int in = 0;
int out = 0;
int empty = MAX_SIZE;
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
            break;
        }
        item = rand();
        pthread_mutex_lock(&buffer_lock);
        if (isFilled || filled == MAX_SIZE)
        {
            pthread_cond_wait(&buffer_queue, &buffer_lock);
        }
        if (buckets[in] == 0)
        {
            buckets[in] = item;
            in = (in + 1) % MAX_SIZE;
            filled++;
        }
        isFilled = true;
        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&buffer_queue);
    }

    pthread_mutex_lock(&finished_lock);

    finished = true;

    pthread_mutex_unlock(&finished_lock);

    pthread_exit(NULL);
}

void *consumer(void *index)
{
    while (true)
    {
        pthread_mutex_lock(&finished_lock);
        if (finished)
        {
            pthread_mutex_unlock(&finished_lock);
            break;
        }
        pthread_mutex_unlock(&finished_lock);

        pthread_mutex_lock(&buffer_lock);
        if (!isFilled || filled == 0)
        {
            pthread_cond_wait(&buffer_queue, &buffer_lock);
        }

        if (buckets[out] != 0)
        {
            int item = buckets[out];
            buckets[out] = 0;
            filled--;
            printf("Consumer %d: Remove Item %d From Bucket %d\n", *((int *)index), item, out);
            out = (out + 1) % MAX_SIZE;
        }
        isFilled = false;
        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&buffer_queue);
    }

    pthread_exit(NULL);
}

int main()
{
    begin = time(NULL);

    pthread_t prod_threads[5], cons_threads[5];

    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&buffer_queue, NULL);

    int thread[MAX_THREADS] = {1, 2, 3, 4, 5};

    for (i = 0; i < MAX_SIZE; i++)
    {
        buckets[i] = 0;
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

    pthread_mutex_destroy(&buffer_lock);

    return EXIT_SUCCESS;
}