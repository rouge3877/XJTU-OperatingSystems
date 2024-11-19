#ifndef API_H
#define API_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

pid_t Fork();
int Kill(pid_t pid, int sig);
pid_t Wait(int *status);
pid_t Waitpid(pid_t pid, int *status, int options);
int Lockf(int fd, int cmd, off_t len);

#endif // API_H