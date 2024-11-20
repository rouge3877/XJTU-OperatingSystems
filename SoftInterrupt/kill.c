#include "API.h"

#define INTER -1
#define WAITING 4

volatile atomic_int flag = 0;
sigset_t global_mask;

void inter_handler()
{
    sigset_t prev_mask;

    Sigprocmask(SIG_BLOCK, &global_mask, &prev_mask); /* Block sigs */
    flag = INTER;
#ifdef DEBUG
    write(STDOUT_FILENO, "Handler received signal!!\n", 27);
#endif
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); /* Restore sigs */

    return;
}
void waiting()
{
    flag += WAITING;
    return;
}
int main()
{
    Sigfillset(&global_mask);
    Sigdelset(&global_mask, SIGUSR1);
    Sigdelset(&global_mask, SIGUSR2);
    Sigprocmask(SIG_SETMASK, &global_mask, NULL);
    Signal(SIGUSR1, waiting);
    Signal(SIGUSR2, waiting);

    pid_t pid1 = -1, pid2 = -1;
    while (pid1 == -1)
        pid1 = Fork();
    if (pid1 > 0)
    {
        while (pid2 == -1)
            pid2 = Fork();
        if (pid2 > 0)
        {
            sigset_t mask;
            while (flag != 2 * WAITING)
                Sigsuspend(&mask);

            Sigfillset(&mask);
            Sigdelset(&mask, SIGALRM);
            Sigdelset(&mask, SIGQUIT);
            Sigdelset(&mask, SIGINT);
            Sigprocmask(SIG_SETMASK, &mask, NULL);
            // Now mask is set only receive SIGALRM, SIGQUIT, SIGINT

            (void)Alarm(5);
            Signal(SIGALRM, inter_handler);
            Signal(SIGQUIT, inter_handler);
            Signal(SIGINT, inter_handler);

#ifdef DEBUG
            printf("Parent process set mask and handler!!\n");
#endif
            while (flag != INTER)
                Sigsuspend(&mask);

#ifdef DEBUG
            printf("Parent process received signal!!\n");
#endif

            Kill(pid1, SIGSTKFLT);
            Waitpid(pid1, NULL, 0);
            Kill(pid2, SIGCHLD);
            Waitpid(pid2, NULL, 0);

            printf("\nParent process is killed!!\n");
        }
        else
        {
            Signal(SIGCHLD, waiting);
            sigset_t mask;
            Sigfillset(&mask);
            Sigdelset(&mask, SIGCHLD);
            Sigprocmask(SIG_SETMASK, &mask, NULL);
            // Now mask is set only receive SIGSTKFLT
            Kill(getppid(), SIGUSR2); // Send signal to parent

            Pause();
            printf("\nChild process2 is killed by parent!!\n");
            return 0;
        }
    }
    else
    {
        Signal(SIGSTKFLT, waiting);
        sigset_t mask;
        Sigfillset(&mask);
        Sigdelset(&mask, SIGSTKFLT);
        Sigprocmask(SIG_SETMASK, &mask, NULL);
        // Now mask is set only receive SIGSTKFLT
        Kill(getppid(), SIGUSR1); // Send signal to parent

        Pause();
        printf("\nChild process1 is killed by parent!!\n");
        return 0;
    }
    return 0;
}