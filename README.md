# XJTU OS Lab1

Rouge Lin | 2024.10.29

This is the [1/3] lab of XJTU OS Experiment (COMP451005).

## Work Summary
### PART1: Process Related ...
This part of the lab is about process (especially, process programming) and process management. 

Following files are included in this lab:
- `PID.c`: This file is about the process ID (PID) and the parent-child relationship of processes. It prints the PID of the parent process and the child process.
   
    output:
    ```
    > ./PID
    parent: pid = 60012
    child: pid = 0
    parent: pid1 = 60011
    child: pid1 = 60012
    ```
- `PID_nowait`: In the file `PID.c`, the parent process `wait(NULL)` for the child process to finish. In this file, the parent process does not wait for the child process to finish.
   
    output:
    ```
    > ./PID_nowait
    parent: pid = 60115
    child: pid = 0
    parent: pid1 = 60114
    child: pid1 = 60115
    ```
    I use `for i in {1..10}; do ./PID_nowait; echo -e '\n---\n'; done` to run the program for 10 times to show the difference. But because the parent process does not wait for the child process to finish, so sometimes the echo command will print the output during the child process is running. That's because the parent process may will finish before the child process. **So the shell will print the output of the echo command before the child process finishes.**

- `glbVar.c`: This file is about the global variable in the parent process and the child process. It prints the value of the global variable in the parent process and the child process.

    output:
    ```
    > ./glbVar
    parent: global = 5
    parent: &global = 0x420048
    child: global = 7
    child: &global = 0x420048
    before return, global = global * 2 = 14
    before return, global = global * 2 = 10
    ```

- `system.c`: This file is about the system call `system()`. It uses the `system()` function in a `fork`ed child process to execute a program `system_call.c` in the system. `system_call.c` is a simple program that prints its PID and PPID.

    output:
    ```
    > ./system
    parent: pid = 60693
    parent: pid1 = 60691
    child: pid = 0
    child: pid1 = 60693
    In File system_call: pid = 60694
    In File system_call: ppid = 60693
    ```

- `exec.c`: This file aims to use the `exec()` (to be specific, `ececlp()`) function in a `fork`ed child process to execute a program `system_call.c` in the system. `system_call.c` is a simple program that prints its PID and PPID.

    output:
    ```
    > ./exec
    parent: pid = 60817
    child: pid = 0
    parent: pid1 = 60816
    child: pid1 = 60817
    In File system_call: pid = 60817
    In File system_call: ppid = 60816
    ```

### Thread Related ...
This part of the lab is about thread programming, creation, mutex, and synchronization.

Following files are related to this part:
- `naive_thread.c`: This file is about the naive thread creation and without any synchronization and mutex. It creates two threads and a global variable `global = 0` is shared by these two threads. Then, one of the threads continued increase the `global` and the other one continued decrease the `global`. Both of them do this for 5000 times. Finally, the main thread prints the value of the `global`.

    output:
    ```
    > ./naive_thread
    sum = -152 # the value of global is not definite in each run
    ```
    
- `mutex.c`: In this file, compared to `naive_thread.c`, a mutex is added to protect the global variable `global`. The two threads will lock the mutex before they access the global variable `global` and unlock the mutex after they finish the access.

    output:
    ```
    > ./mutex
    sum = 0
    ```

- `semaphore.c`: In this file, a semaphore is used to synchronize the two threads. The two threads will wait for the semaphore before they access the global variable `global` and post the semaphore after they finish the access.

    output:
    ```
    > ./semaphore
    sum = 0
    ```

- `system_thread.c`: This file is about the system call `system()` in a thread. It uses the `system()` function in all created thread (2 created thread here) to execute a program `getTID.c` in the system. `getTID.c` is a simple program that prints its PID and TID. And for sure, all the threads will print their PID and TID.

    output:
    ```
    > ./system_thread
    PID: 61941
    TID: 61942
    PID: 61941
    TID: 61943
    getTID: PID: 61944
    getTID: TID: 61944
    getTID: PID: 61945
    getTID: TID: 61945
    ```

- `exec_thread.c`: This file is about using the `exec()` (specifically, `e ce c l p |()`) function in a thread. It uses the `exec()` function in all created thread (2 created thread here) to execute a program `getTID.c` in the system. `getTID.c` is a simple program that prints its PID and TID. And for sure, all the threads will print their PID and TID.

    output:
    ```
    > ./exec_thread
    PID: 62481
    TID: 62482
    PID: 62481
    TID: 62483
    getTID: PID: 62481
    getTID: TID: 62481
    ```
    To be noticed, the `exec()` function will replace the current process with the new process. So, the thread that calls the `exec()` function will be terminated the original process (**that is, another thread will not be created to execute the new process**). And in this way, the new process will have the same PID as the original process.

### PART3: SPIN LOCK (@`spin.c`)
This part of the lab is about the spin lock. The spin lock is a lock that waits for the lock to be released in a loop (spin) until the lock is acquired.

For the given frame code `spin.c` which the spin lock is implemented, My work is to complete the `main()` function by adding the spin lock to protect the global variable `shared_value`. The two threads will increase the `shared_value` for 5000 times. The main thread will print the final value of the `shared_value`.

    output:
    ```
    > ./spin
    Begin: shared_value = 0
    Final: shared_value = 10000
    ```

## More Details
1. Concurrent programming is hard. And for the same code, the output may be different in different runs. For example, in `naive_thread.c`, the value of the global variable `global` is not definite in each run. So in the makefile, I use `for i in {1..10}; do ./naive_thread; done` to run the program for 10 times to show the difference.

2. The `system()` function is used to execute a program in the system. The `exec()` function is used to replace the current process with a new process. So, the `system()` function will create a new process to execute the program, and the `exec()` function will replace the current process with the new process.

3. In Linux, process and thread are both implemented by `task_struct`. The difference between process and thread is that they have different memory space. But in the user space, they are similar. So, the `fork()` function will create a new process, and the `pthread_create()` function will create a new thread.
