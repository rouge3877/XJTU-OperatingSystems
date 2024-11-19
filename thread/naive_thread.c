#include <stdio.h>
#include <pthread.h>

int sum = 0;
#define TIMES 5000

void *thread1(void *arg)
{
    int i;
    for (i = 0; i < TIMES; i++)
    {
        sum += 1;
    }
    pthread_exit(0);
}

void *thread2(void *arg)
{
    int i;
    for (i = 0; i < TIMES; i++)
    {
        sum -= 1;
    }
    pthread_exit(0);
}

int main()
{
    pthread_t id1, id2;
    pthread_create(&id1, NULL, thread1, NULL);
    pthread_create(&id2, NULL, thread2, NULL);
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    printf("sum = %d\n", sum);
    return 0;
}

