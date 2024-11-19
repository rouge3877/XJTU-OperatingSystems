// get TID and PID of a process
# include <stdio.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/syscall.h>
int main() {
    pid_t pid;
    pid_t tid;
    pid = getpid();
    tid = syscall(SYS_gettid);
    printf("getTID: PID: %d\n", pid);
    printf("getTID: TID: %d\n", tid);
    return 0;
}
