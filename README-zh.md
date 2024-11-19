# XJTU 操作系统实验1

**李雨轩 | 2024.10.29**

这是西安交通大学操作系统实验（COMP451005）系列中的第 [1/3] 个实验。

## 工作总结
### PART1: 进程相关实验
本部分实验集中于进程的概念和操作，尤其是进程编程和进程管理，涵盖进程的创建、父子关系、系统调用等。

本实验包含以下文件：

- `PID.c`：该文件展示了进程ID（PID）的概念以及进程的父子关系。程序在执行时会分别打印出父进程和子进程的PID，以便观察每次创建的新进程和其父进程之间的联系。
  
    输出示例：
    ```
    > ./PID
    parent: pid = 60012
    child: pid = 0
    parent: pid1 = 60011
    child: pid1 = 60012
    ```
    printf buffer problem, add \n to solve

- `PID_nowait`：在 `PID.c` 中，父进程会通过 `wait(NULL)` 等待子进程完成。而在该文件中，父进程不等待子进程完成，直接继续执行。该设计用于观察父进程不等待子进程的情况下，二者输出的顺序和状态差异。

    输出示例：
    ```
    > ./PID_nowait
    parent: pid = 60115
    child: pid = 0
    parent: pid1 = 60114
    child: pid1 = 60115
    ```
    为展示这种不同，我通过命令 `for i in {1..10}; do ./PID_nowait; echo -e '\n---\n'; done` 多次运行程序。由于父进程不等待子进程完成，父进程可能在子进程完成之前就结束，因此 echo 命令的输出可能在子进程的输出之前出现。

- `glbVar.c`：该文件用于演示父进程和子进程中全局变量的行为。在父进程和子进程中打印全局变量的值，可以观察到全局变量的存储位置不变，但变量值在父子进程间的相互独立性。

    输出示例：
    ```
    > ./glbVar
    parent: global = 5
    parent: &global = 0x420048
    child: global = 7
    child: &global = 0x420048
    before return, global = global * 2 = 14
    before return, global = global * 2 = 10
    ```

- `system.c`：该文件展示了系统调用 `system()` 的用法，通过 `fork` 创建子进程并在其中调用 `system()` 执行程序 `system_call.c`，该程序会打印其PID和父进程的PID（PPID），展示系统调用创建的进程信息。

    输出示例：
    ```
    > ./system
    parent: pid = 60693
    parent: pid1 = 60691
    child: pid = 0
    child: pid1 = 60693
    In File system_call: pid = 60694
    In File system_call: ppid = 60693
    ```

- `exec.c`：此文件演示了 `exec()` 系列函数（具体为 `execlp()`）的用法。它通过在子进程中调用 `exec()`，直接执行程序 `system_call.c`，从而将子进程替换为 `system_call.c` 程序，展示 `exec()` 函数如何完全覆盖并替换当前进程。

    输出示例：
    ```
    > ./exec
    parent: pid = 60817
    child: pid = 0
    parent: pid1 = 60816
    child: pid1 = 60817
    In File system_call: pid = 60817
    In File system_call: ppid = 60816
    ```

### PART2: 线程相关实验
本部分实验涉及线程的创建、同步及其与全局变量的交互，重点演示多线程编程中的竞争条件、互斥锁和信号量。

相关文件包括：

- `naive_thread.c`：该文件演示简单的多线程创建。程序中两个线程共享全局变量 `global = 0`，一个线程不断增加该变量，另一个线程不断减少，每个线程均执行5000次操作。由于未使用任何同步机制，每次运行 `global` 的值不固定，展示了竞争条件对结果的影响。

    输出示例：
    ```
    > ./naive_thread
    sum = -152 # 每次运行 global 的值不确定
    ```

- `mutex.c`：在此文件中，通过互斥锁（mutex）保护全局变量 `global`，避免了竞争条件。两个线程在访问 `global` 时先锁定互斥锁，访问完成后解锁，以确保每次操作的原子性。

    输出示例：
    ```
    > ./mutex
    sum = 0
    ```

- `semaphore.c`：该文件利用信号量来实现线程同步。两个线程在访问 `global` 前等待信号量，操作完成后释放信号量，确保线程间的有序访问。

    输出示例：
    ```
    > ./semaphore
    sum = 0
    ```

- `system_thread.c`：此文件演示了在多线程环境中使用系统调用 `system()`。通过在两个线程中分别调用 `system()` 执行程序 `getTID.c`，展示系统调用如何影响每个线程的PID和TID。

    输出示例：
    ```
    > ./system_thread
    PID: 61941
    TID: 61942
    PID: 61941
    TID: 61943
    getTID: PID: 61944
    getTID: TID: 61944
    ```

- `exec_thread.c`：此文件在多线程环境中演示 `exec()` 函数的使用。通过 `exec()` 执行程序 `getTID.c`，可以观察到调用 `exec()` 的线程被新进程替换，并不再执行原进程中的其他线程。

    输出示例：
    ```
    > ./exec_thread
    PID: 62481
    TID: 62482
    getTID: PID: 62481
    getTID: TID: 62481
    ```

### PART3: 自旋锁（@`spin.c`）
本部分实验探讨了自旋锁的实现与使用。自旋锁通过持续循环等待的方式直到获取锁。在 `spin.c` 中，通过自旋锁保护全局变量 `shared_value`，并在两个线程中对其进行5000次增加操作，以确保 `shared_value` 在并发条件下的正确性。

    输出示例：
    ```
    > ./spin
    Begin: shared_value = 0
    Final: shared_value = 10000
    ```

## 详细说明
1. 并发编程的输出可能会因竞争条件而不稳定。在 `naive_thread.c` 中，全局变量 `global` 的值每次运行并不一致，因此使用 `for i in {1..10}; do ./naive_thread; done` 以观察不同输出。

2. `system()` 函数用于在系统中启动新进程执行程序，而 `exec()` 函数则将当前进程替换为新进程，前者会创建新进程，后者则覆盖当前进程。

3. 在 Linux 系统中，进程和线程均通过 `task_struct` 实现，二者的内存空间不同，但在用户空间具有类似行为。
