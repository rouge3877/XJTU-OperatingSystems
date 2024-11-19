#include <stdio.h>
#include <pthread.h>

int sum = 0;
pthread_mutex_t mutex;

void *thread1(void *arg)
{
    for (int i = 0; i < 5000; i++)
    {
        pthread_mutex_lock(&mutex);
        sum += 1;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void *thread2(void *arg)
{
    for (int i = 0; i < 5000; i++)
    {
        pthread_mutex_lock(&mutex);
        sum -= 1;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t id1, id2;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&id1, NULL, thread1, NULL);
    pthread_create(&id2, NULL, thread2, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("sum = %d\n", sum);

    pthread_mutex_destroy(&mutex);

    return 0;
}
