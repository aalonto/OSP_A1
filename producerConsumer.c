#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

const int MAX_SIZE = 10;
const int MAX_THREADS = 5;

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

bool buffer_full = false;
pthread_cond_t buffer_queue = PTHREAD_COND_INITIALIZER;

pthread_mutex_t finished_lock = PTHREAD_MUTEX_INITIALIZER;
bool finished = false;

int i;
int in = 0;
int out = 0;
int empty = MAX_SIZE;
int filled = 0;
const int RUNTIME = 10;
time_t begin, current;

int buckets[MAX_SIZE];

void *producer(void *index)
{
    int item;
    do
    {
        current = time(NULL);
        pthread_mutex_lock(&buffer_lock);
        item = rand();
        while (buffer_full)
        {
            pthread_cond_wait(&buffer_queue, &buffer_lock);
        }

        if (buckets[in] == 0)
        {
            buckets[in] = item;
            printf("Producer %d: Insert Item %d at %d\n", *((int *)index), buckets[in], in);
            in = (in + 1) % MAX_SIZE;
            filled++;

            buffer_full = true;
        }

        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&buffer_queue);

    } while (difftime(current, begin) < RUNTIME);
    // for(i = 0; i < MAX_SIZE; ++i){

    // }

    pthread_mutex_lock(&finished_lock);

    finished = true;

    pthread_mutex_unlock(&finished_lock);

    return NULL;
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
        while (!buffer_full)
        {
            pthread_cond_wait(&buffer_queue, &buffer_lock);
        }
        if (buckets[out] != 0)
        {
            int item = buckets[out];
            buckets[out] = 0;
            filled--;
            printf("Consumer %d: Remove Item %d from %d\n", *((int *)index), item, out);
            out = (out + 1) % MAX_SIZE;
            buffer_full = false;
        }

        pthread_mutex_unlock(&buffer_lock);
        pthread_cond_signal(&buffer_queue);
    }

    return NULL;
}

int main()
{
    pthread_t prod_threads[5], cons_threads[5];

    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&buffer_queue, NULL);

    int thread[MAX_THREADS] = {1, 2, 3, 4, 5};

    for (i = 0; i < MAX_SIZE; i++)
    {
        buckets[i] = 0;
    }

    begin = time(NULL);

    pthread_create(&prod_threads[0], NULL, (void *)producer, (void *)&thread[0]);
    // }

    // for (i = 0; i < MAX_THREADS; i++)
    // {
        pthread_create(&cons_threads[0], NULL, (void *)consumer, (void *)&thread[0]);
    // }

    // for (i = 0; i < MAX_THREADS; i++)
    // {
        pthread_join(prod_threads[0], NULL);
    // }

    // for (i = 0; i < 5; i++)
    // {
        pthread_join(cons_threads[0], NULL);
    // }
    pthread_mutex_destroy(&buffer_lock);
    printf("The elapsed time is %ld seconds\n", (current - begin));

    return EXIT_SUCCESS;
}