#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid = getpid();
    printf("In File system_call: pid = %d\n", pid);
    pid = getppid();
    printf("In File system_call: ppid = %d\n", pid);

    return 0;
}
