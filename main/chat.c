#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define DEVICE_PATH "/dev/ascii_char_device"
#define BUFFER_SIZE 2048

int fd;

// 读取线程函数
void *read_thread_func(void *arg) {
    char buffer[BUFFER_SIZE];
    ssize_t ret;
    pid_t sender_pid, receiver_pid;

    while (1) {
        ret = read(fd, buffer, sizeof(buffer));
        if (ret > 0) {
            size_t offset = 0;
            while (offset < ret) {
                // 读取发送者PID
                memcpy(&sender_pid, buffer + offset, sizeof(pid_t));
                offset += sizeof(pid_t);

                // 读取接收者PID
                memcpy(&receiver_pid, buffer + offset, sizeof(pid_t));
                offset += sizeof(pid_t);

                // 读取消息内容
                char *msg_content = buffer + offset;
                size_t msg_len = strlen(msg_content);
                offset += msg_len + 1; // 包含字符串结束符

                // 打印消息
                if (receiver_pid == 0)
                    printf("[Broadcast] PID %d: %s\n", sender_pid, msg_content);
                else
                    printf("[Private] PID %d to PID %d: %s\n", sender_pid, receiver_pid, msg_content);
            }
        } else if (ret == 0) {
            // 没有新消息
            usleep(100000); // 休眠100ms
        } else {
            perror("Failed to read from device");
            break;
        }
    }
    return NULL;
}

// 发送消息函数
void send_message(const char *message) {
    if (write(fd, message, strlen(message)) < 0) {
        perror("Failed to write to device");
    }
}

// 主函数
int main() {
    pthread_t read_thread;
    char input[BUFFER_SIZE];
    pid_t my_pid = getpid();

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Chat program started. Your PID: %d\n", my_pid);
    printf("Enter messages (for private message, use @<PID> <message>):\n");

    // 创建读取线程
    if (pthread_create(&read_thread, NULL, read_thread_func, NULL) != 0) {
        perror("Failed to create read thread");
        close(fd);
        return EXIT_FAILURE;
    }

    // 主线程用于发送消息
    while (fgets(input, sizeof(input), stdin)) {
        // 去掉换行符
        input[strcspn(input, "\n")] = '\0';

        // 发送消息
        send_message(input);
    }

    // 关闭设备
    close(fd);
    return EXIT_SUCCESS;
}
