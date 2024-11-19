#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int sum = 0;
#define TIMES 5000
sem_t semaphore;

void *thread1(void *arg)
{
    int i;
    for (i = 0; i < TIMES; i++)
    {
        sem_wait(&semaphore);  // P 操作，等待信号量
        sum += 1;
        sem_post(&semaphore);  // V 操作，释放信号量
    }
    pthread_exit(0);
}

void *thread2(void *arg)
{
    int i;
    for (i = 0; i < TIMES; i++)
    {
        sem_wait(&semaphore);  // P 操作，等待信号量
        sum -= 1;
        sem_post(&semaphore);  // V 操作，释放信号量
    }
    pthread_exit(0);
}

int main()
{
    pthread_t id1, id2;

    // 初始化信号量，初始值为 1
    sem_init(&semaphore, 0, 1);

    pthread_create(&id1, NULL, thread1, NULL);
    pthread_create(&id2, NULL, thread2, NULL);
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("sum = %d\n", sum);

    // 销毁信号量
    sem_destroy(&semaphore);
    return 0;
}
