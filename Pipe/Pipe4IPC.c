#include "API.h"

int pid1, pid2; // 定义两个进程变量
int main()
{
    int fd[2];
    char InPipe[4001]; // 定义读缓冲区
    char c1 = '1', c2 = '2';
    (void)Pipe(fd); // 创建管道

    while ((pid1 = Fork()) == -1)
        ; // 如果进程 1 创建不成功,则空循环

    if (pid1 == 0) // 如果子进程 1 创建成功,pid1 为进程号
    {   
        // TODO: 锁定管道
        lockf(fd[1], F_LOCK, 0);
        // TODO:  2000 次每次向管道写入字符’1’
        for(int i = 0; i < 2000; i++)
            write(fd[1], &c1, 1);
        
#ifdef DEBUG
        printf("Child 1: write 2000 c1\n");
#endif

        sleep(5); // 等待读进程读出数据
        // TODO: 解除管道的锁定
        lockf(fd[1], F_ULOCK, 0);
        exit(0); // 结束进程 1
    }
    else
    {
        while ((pid2 = Fork()) == -1)
            ; // 若进程 2 创建不成功,则空循环
        if (pid2 == 0)
        {
            lockf(fd[1], 1, 0);
            // TODO: 分 2000 次每次向管道写入字符’2’
            for(int i = 0; i < 2000; i++)
                write(fd[1], &c2, 1);

#ifdef DEBUG
            printf("Child 2: write 2000 c2\n");
#endif

            sleep(5);
            lockf(fd[1], 0, 0);
            exit(0);
        }
        else
        {
            // TODO: 等待子进程 1 结束
            Waitpid(pid1, NULL, 0);
            Waitpid(pid2, NULL, 0); // 等待子进程 2 结束
            // TODO: 从管道中读出 4000 个字符
            read(fd[0], InPipe, 4000);
            // TODO: 加字符串结束符
            InPipe[4000] = '\0';
            printf("%s\n", InPipe); // 显示读出的数据
            exit(0);                // 父进程结束
        }
    }
    return 0;
}