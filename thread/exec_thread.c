#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/syscall.h>

void *thread1(void *arg) {
    pid_t pid;
    pid_t tid;
    pid = getpid();
    tid = syscall(SYS_gettid);
    printf("PID: %d\n", pid);
    printf("TID: %d\n", tid);
    execlp("./getTID", "getTID", NULL);
    return NULL;
}

void *thread2(void *arg) {
    pid_t pid;
    pid_t tid;
    pid = getpid();
    tid = syscall(SYS_gettid);
    printf("PID: %d\n", pid);
    printf("TID: %d\n", tid);
    execlp("./getTID", "getTID", NULL);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}