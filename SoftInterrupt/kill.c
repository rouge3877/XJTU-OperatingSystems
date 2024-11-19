#include <API.h>

int flag = 0; // 用于标识信号是否已触发

// 信号处理函数
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGQUIT) {
        flag = sig; // 捕获信号类型
    }
}

// 子进程等待信号的函数
void waiting() {
    while (flag == 0) {
        sleep(1); // 通过循环检查flag，阻塞子进程
    }
}

int main() {
    pid_t pid1 = -1, pid2 = -1;

    // 捕获信号 SIGINT 和 SIGQUIT
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    while (pid1 == -1) pid1 = fork(); // 创建第一个子进程

    if (pid1 > 0) { // 父进程
        while (pid2 == -1) pid2 = fork(); // 创建第二个子进程

        if (pid2 > 0) { // 父进程继续执行
            printf("Parent process is waiting for a signal...\n");
            for (int i = 0; i < 5 && flag == 0; ++i) {
                sleep(1); // 等待5秒钟，期间监听信号
                if(flag == 4){
                    Kill(pid1, SIGALRM);
                    Kill(pid2, SIGALRM);
                    break;
                }
            }

            if (flag == SIGINT || flag == SIGQUIT) {
                // 向两个子进程发送信号
                printf("Parent received signal: %s\n", flag == SIGINT ? "SIGINT" : "SIGQUIT");
                kill(pid1, SIGUSR1); // 通知子进程 1
                waitpid(pid1, NULL, 0); // 等待子进程 1 结束

                kill(pid2, SIGUSR2); // 通知子进程 2
                waitpid(pid2, NULL, 0); // 等待子进程 2 结束
            }

            printf("Parent process is killed!!\n");
        } else { // 子进程2
            signal(SIGUSR2, signal_handler); // 捕获来自父进程的信号
            waiting();
            printf("Child process 2 is killed by parent!!\n");
            return 0;
        }
    } else { // 子进程1
        signal(SIGUSR1, signal_handler); // 捕获来自父进程的信号
        waiting();
        printf("Child process 1 is killed by parent!!\n");
        return 0;
    }

    return 0;
}
