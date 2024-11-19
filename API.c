#include "API.h"

pid_t Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int Kill(pid_t pid, int sig) {
    int result = kill(pid, sig);
    if (result < 0) {
        perror("kill failed");
        exit(EXIT_FAILURE);
    }
    return result;
}

pid_t Wait(int *status) {
    pid_t pid = wait(status);
    if (pid < 0) {
        perror("wait failed");
        exit(EXIT_FAILURE);
    }
    return pid;
}

pid_t Waitpid(pid_t pid, int *status, int options) {
    pid_t result = waitpid(pid, status, options);
    if (result < 0) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }
    return result;
}

int Lockf(int fd, int cmd, off_t len) {
    int result = lockf(fd, cmd, len);
    if (result < 0) {
        perror("lockf failed");
        exit(EXIT_FAILURE);
    }
    return result;
}
