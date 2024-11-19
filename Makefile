# XJTU OS Lab1 Makefile

all: build_dirs part1 part2 part3

clean:
	rm -rf build

# 创建输出文件夹
build_dirs:
	mkdir -p build/part1 build/part2 build/part3

# Part 1: Process Related
part1: build/part1/PID build/part1/PID_nowait build/part1/glbVar build/part1/system build/part1/exec

build/part1/PID: process/PID.c
	gcc process/PID.c -o build/part1/PID

build/part1/PID_nowait: process/PID.c
	sed '/wait(NULL);/d' process/PID.c > PID_tmp.c
	gcc PID_tmp.c -o build/part1/PID_nowait
	rm -f PID_tmp.c

build/part1/glbVar: process/glbVar.c
	gcc process/glbVar.c -o build/part1/glbVar

build/part1/system: process/system.c process/system_call.c
	gcc process/system_call.c -o build/part1/system_call
	gcc process/system.c -o build/part1/system

build/part1/exec: process/exec.c process/system_call.c
	gcc process/exec.c -o build/part1/exec
	gcc process/system_call.c -o build/part1/system_call

# Part 2: Thread Related
part2: build/part2/naive_thread build/part2/mutex build/part2/semaphore build/part2/system_thread build/part2/exec_thread

build/part2/naive_thread: thread/naive_thread.c
	gcc thread/naive_thread.c -o build/part2/naive_thread -lpthread

build/part2/mutex: thread/mutex.c
	gcc thread/mutex.c -o build/part2/mutex -lpthread

build/part2/semaphore: thread/semaphore.c
	gcc thread/semaphore.c -o build/part2/semaphore -lpthread

build/part2/system_thread: thread/system_thread.c thread/getTID.c
	gcc thread/getTID.c -o build/part2/getTID
	gcc thread/system_thread.c -o build/part2/system_thread -lpthread

build/part2/exec_thread: thread/exec_thread.c thread/getTID.c
	gcc thread/exec_thread.c -o build/part2/exec_thread -lpthread
	gcc thread/getTID.c -o build/part2/getTID

# Part 3: Spin Lock
part3: build/part3/spin

build/part3/spin: spin.c
	gcc spin.c -o build/part3/spin -lpthread

# 批量运行命令
run_pid:
	@for i in {1..10}; do build/part1/PID; echo -e "\n--- ---\n"; done

run_pid_nowait:
	@for i in {1..10}; do build/part1/PID_nowait; echo -e "\n--- ---\n"; done

run_glbvar:
	@for i in {1..10}; do build/part1/glbVar; echo -e "\n--- ---\n"; done

run_system:
	@for i in {1..10}; do build/part1/system; echo -e "\n--- ---\n"; done

run_exec:
	@for i in {1..10}; do build/part1/exec; echo -e "\n--- ---\n"; done

run_naive_thread:
	@for i in {1..10}; do build/part2/naive_thread; echo -e "--- ---"; done

run_mutex:
	@for i in {1..10}; do build/part2/mutex; echo -e "--- ---"; done

run_semaphore:
	@for i in {1..10}; do build/part2/semaphore; echo -e "--- ---"; done

run_system_thread:
	@for i in {1..10}; do build/part2/system_thread; echo -e "--- ---"; done

run_exec_thread:
	@for i in {1..10}; do build/part2/exec_thread; echo -e "--- ---"; done

run_spin:
	@for i in {1..10}; do build/part3/spin; echo -e "--- ---"; done
