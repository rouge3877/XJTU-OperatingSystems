#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
int main()
{
    struct timeval tv;
    syscall(__NR_gettimeofday, &tv, NULL); // before modify syscall 78 :gettimeofday
    printf("tv_sec:%d\n", tv.tv_sec);
    printf("tv_usec:%d\n", tv.tv_usec);
    return 0;
}