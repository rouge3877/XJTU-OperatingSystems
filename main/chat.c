#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define DEVICE_PATH "/dev/ascii_char_device"  // Device file path
#define BUFFER_SIZE 2048  // Buffer size for reading data

int fd;  // File descriptor for the device

// Read thread function
void *read_thread_func(void *arg) {
    ssize_t ret;
    pid_t sender_pid, receiver_pid;

    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        ret = read(fd, buffer, sizeof(buffer));  // Read data from the device

        // Process the read data
        if (ret > 0) {
            size_t offset = 0;
            while (offset < ret) {
                // Read the sender's PID
                memcpy(&sender_pid, buffer + offset, sizeof(pid_t));
                offset += sizeof(pid_t);

                // Read the receiver's PID
                memcpy(&receiver_pid, buffer + offset, sizeof(pid_t));
                offset += sizeof(pid_t);

                // Read the message content
                char *msg_content = buffer + offset;
                size_t msg_len = strlen(msg_content);
                offset += msg_len + 1;  // Include the null terminator

                // Print debug information (if DEBUG is defined)
#ifdef DEBUG
                printf("[debug]: msg_len: %zu\n", msg_len);
                printf("[debug]: msg_content: %s\n", msg_content);
                printf("[debug]: receiver_pid: %d\n", receiver_pid);
                printf("[debug]: sender_pid: %d\n", sender_pid);
#endif

                // Print the message
                if (receiver_pid == 0)
                    printf("[Broadcast] PID %d: %s\n", sender_pid, msg_content);
                else
                    printf("[Private] PID %d to PID %d: %s\n", sender_pid, receiver_pid, msg_content);
            }
        } else if (ret == 0) {
            // No new message, sleep for 100ms
            usleep(100000);
        } else {
            // Error reading from device, exit the thread
            perror("Failed to read from device");
            break;
        }
    }
    return NULL;
}

// Function to send a message
void send_message(const char *message) {
    ssize_t written = write(fd, message, strlen(message));
    if (written < 0) {
        perror("Failed to write to device");
    }

    // Print debug information (if DEBUG is defined)
#ifdef DEBUG
    printf("[debug]: Sent message: %s\n", message);
#endif
}

int main() {
    pthread_t read_thread;
    char input[BUFFER_SIZE];
    pid_t my_pid = getpid();  // Get the current process PID

    // Open the device file for read and write access
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Chat program started. Your PID: %d\n", my_pid);
    printf("Enter messages (for private message, use @<PID> <message>):\n");

    // Create the read thread
    if (pthread_create(&read_thread, NULL, read_thread_func, NULL) != 0) {
        perror("Failed to create read thread");
        close(fd);
        return EXIT_FAILURE;
    }

    // Main thread is used for sending messages
    while (fgets(input, sizeof(input), stdin)) {
        // Remove the newline character from the input
        input[strcspn(input, "\n")] = '\0';

        // Send the message
        send_message(input);
    }

    // Close the device file before exiting
    close(fd);
    return EXIT_SUCCESS;
}
