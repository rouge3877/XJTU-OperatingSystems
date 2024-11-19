#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 6;

int main()
{
    pid_t pid, pid1;

    /* fork a child process */
    pid = fork();

    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* child process */
        global++;
        printf("child: global = %d\n", global);
        printf("child: &global = %p\n", &global);
    }
    else { /* parent process */
        global--;
        printf("parent: global = %d\n", global);
        printf("parent: &global = %p\n", &global);
        wait(NULL);
    }

    global *= 2;
    printf("before return, global = global * 2 = %d\n", global);

    return 0;
}

