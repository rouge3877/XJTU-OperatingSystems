#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>      // copy_to_user, copy_from_user
#include <linux/cdev.h>
#include <linux/slab.h>         // kmalloc, kfree
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/atomic.h>

#define DEVICE_NAME "ascii_char_device"
#define BUFFER_SIZE 1024            // 最大消息数
#define MESSAGE_CONTENT_SIZE 512    // 单条消息内容大小

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rouge Lin");
MODULE_DESCRIPTION("ascii_char_device with improved chat functionality and read count");
MODULE_VERSION("1.3");


// 消息结构体
struct message {
    pid_t sender_pid;
    pid_t receiver_pid; // 0 表示广播消息
    char content[MESSAGE_CONTENT_SIZE];
    struct list_head list;
    unsigned int read_count; // 已读取该消息的用户数
};

// 设备结构体
struct ascii_char_device {
    struct cdev cdev;
    struct list_head msg_list;      // 消息链表
    size_t msg_count;
    struct semaphore sem;           // 读写互斥信号量
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
    atomic_t user_count;            // 当前打开设备的用户数
} ascii_device;

// 主设备号
static int major_number;

// 文件操作函数声明
static int ascii_open(struct inode *inode, struct file *file);
static int ascii_release(struct inode *inode, struct file *file);
static ssize_t ascii_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t ascii_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

// 文件操作结构体
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = ascii_open,
    .release = ascii_release,
    .read = ascii_read,
    .write = ascii_write,
};

// 打开设备
static int ascii_open(struct inode *inode, struct file *file) {
    struct ascii_char_device *dev;

    dev = container_of(inode->i_cdev, struct ascii_char_device, cdev);
    file->private_data = dev;

    // 增加用户计数
    atomic_inc(&dev->user_count);

    printk(KERN_INFO "ascii_char_device: Device opened by PID %d (Total users: %d)\n", current->tgid, atomic_read(&dev->user_count));
    return 0;
}

// 释放设备
static int ascii_release(struct inode *inode, struct file *file) {
    struct ascii_char_device *dev = file->private_data;

    // 减少用户计数
    atomic_dec(&dev->user_count);

    printk(KERN_INFO "ascii_char_device: Device closed by PID %d (Total users: %d)\n", current->tgid, atomic_read(&dev->user_count));
    return 0;
}

// 读取设备
static ssize_t ascii_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
    struct ascii_char_device *dev = file->private_data;
    struct message *msg;
    struct list_head *pos, *tmp;
    char kernel_buf[MESSAGE_CONTENT_SIZE + sizeof(pid_t) * 2];
    ssize_t bytes_read = 0;
    int total_bytes = 0;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    // 等待有新消息
    while (dev->msg_count == 0) {
        up(&dev->sem);
        if (wait_event_interruptible(dev->read_queue, dev->msg_count > 0))
            return -ERESTARTSYS;
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    // 遍历消息链表，找到符合条件的消息
    list_for_each_safe(pos, tmp, &dev->msg_list) {
        msg = list_entry(pos, struct message, list);

        // 过滤私聊消息
        printk(KERN_DEBUG "[DEBUG]: ------------------------------------------\n");
        printk(KERN_DEBUG "[DEBUG]: Read message:  msg->receiver_pid = %d\n", msg->receiver_pid);
        printk(KERN_DEBUG "[DEBUG]: Read message:  current->tgid = %d\n", current->tgid);
        printk(KERN_DEBUG "[DEBUG]: Read message:  msg->sender_pid = %d\n", msg->sender_pid);
        printk(KERN_DEBUG "[DEBUG]: ------------------------------------------\n");

        if (msg->receiver_pid != 0 && msg->receiver_pid != current->tgid && msg->sender_pid != current->tgid) {
            continue;
        }

        // 清零 kernel_buf 以防止残留数据
        memset(kernel_buf, 0, sizeof(kernel_buf));

        // 构造用户缓冲区数据：发送者PID + 接收者PID + 内容
        *((pid_t *)kernel_buf) = msg->sender_pid;
        *((pid_t *)(kernel_buf + sizeof(pid_t))) = msg->receiver_pid;
        strncpy(kernel_buf + 2 * sizeof(pid_t), msg->content, MESSAGE_CONTENT_SIZE - 1);
        
        printk(KERN_DEBUG "[DEBUG]: Read message:  msg->content = %s\n", msg->content);

        kernel_buf[2 * sizeof(pid_t) + MESSAGE_CONTENT_SIZE - 1] = '\0';

        // 计算实际读取的字节数
        bytes_read = sizeof(pid_t) * 2 + strlen(kernel_buf + 2 * sizeof(pid_t));
        if (total_bytes + bytes_read > count){
            printk(KERN_INFO "ascii_char_device: User buffer is full\n");
            break;
        }

        // 拷贝到用户空间
        printk(KERN_DEBUG "[DEBUG]: Copying to user space:  kernel_buf = %s\n", kernel_buf+2*sizeof(pid_t));

        if (copy_to_user(buf + total_bytes, kernel_buf, bytes_read)) {
            up(&dev->sem);
            return -EFAULT;
        }

        total_bytes += bytes_read;

        // 更新读取计数
        msg->read_count += 1;

        // 如果所有用户都已读取该消息，则删除它
        if (msg->read_count >= atomic_read(&dev->user_count)) {
            list_del(&msg->list);
            kfree(msg);
            dev->msg_count--;
            printk(KERN_INFO "ascii_char_device: Message from PID %d deleted after being read by all users\n", msg->sender_pid);
        }else if(msg->receiver_pid !=0 && msg->receiver_pid == current->tgid){
            list_del(&msg->list);
            kfree(msg);
            dev->msg_count--;
            printk(KERN_INFO "ascii_char_device: Private message from PID %d deleted after being read by receiver\n", msg->sender_pid);
        }
    }

    up(&dev->sem);

    if (total_bytes > 0) {
        printk(KERN_INFO "ascii_char_device: Read %ld bytes by PID %d\n", total_bytes, current->tgid);
        return total_bytes;
    }

    return 0;
}

// 写入设备
static ssize_t ascii_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    struct ascii_char_device *dev = file->private_data;
    struct message *msg;
    char kernel_buf[MESSAGE_CONTENT_SIZE];
    char *at_ptr;
    pid_t target_pid = 0;
    ssize_t bytes_written = 0;

    if (count > MESSAGE_CONTENT_SIZE - 1)
        count = MESSAGE_CONTENT_SIZE - 1;

    // 清零 kernel_buf 以防止残留数据
    memset(kernel_buf, 0, sizeof(kernel_buf));

    if (copy_from_user(kernel_buf, buf, count))
        return -EFAULT;
    
    kernel_buf[count] = '\0';

    // 检查是否为私聊消息
    if (kernel_buf[0] == '@') {
        at_ptr = strchr(kernel_buf, ' ');
        if (at_ptr) {
            *at_ptr = '\0';
            target_pid = simple_strtol(kernel_buf + 1, NULL, 10);
            at_ptr++;
            strncpy(kernel_buf, at_ptr, MESSAGE_CONTENT_SIZE - 1);
            kernel_buf[MESSAGE_CONTENT_SIZE - 1] = '\0';
        } else {
            // 格式错误，忽略
            return -EINVAL;
        }
    }

    // 分配新的消息结构
    msg = kmalloc(sizeof(struct message), GFP_KERNEL);
    if (!msg)
        return -ENOMEM;

    msg->sender_pid = current->tgid;
    msg->receiver_pid = target_pid;
    strncpy(msg->content, kernel_buf, MESSAGE_CONTENT_SIZE - 1);
    msg->content[MESSAGE_CONTENT_SIZE - 1] = '\0';
    msg->read_count = 0; // 初始化读取计数
    INIT_LIST_HEAD(&msg->list);

    if (down_interruptible(&dev->sem)) {
        kfree(msg);
        return -ERESTARTSYS;
    }

    // 检查缓冲区是否已满
    if (dev->msg_count >= BUFFER_SIZE) {
        // 删除最旧的消息
        struct message *old_msg = list_first_entry(&dev->msg_list, struct message, list);
        list_del(&old_msg->list);
        kfree(old_msg);
        dev->msg_count--;
        printk(KERN_INFO "ascii_char_device: Oldest message deleted to make room for new message\n");
    }

    // 添加新消息到链表尾部
    list_add_tail(&msg->list, &dev->msg_list);
    dev->msg_count++;
    bytes_written = count;

    up(&dev->sem);

    // 唤醒等待的读者
    wake_up_interruptible(&dev->read_queue);

    printk(KERN_INFO "ascii_char_device: Written by PID %d, receiver PID %d\n", msg->sender_pid, msg->receiver_pid);
    return bytes_written;
}

// 模块初始化函数
static int __init ascii_init(void) {
    dev_t dev;
    int ret;

    // 初始化消息链表
    INIT_LIST_HEAD(&ascii_device.msg_list);
    ascii_device.msg_count = 0;
    atomic_set(&ascii_device.user_count, 0);

    // 分配主设备号
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "ascii_char_device: Failed to allocate major number\n");
        return ret;
    }
    major_number = MAJOR(dev);
    printk(KERN_INFO "ascii_char_device: Registered with major number %d\n", major_number);

    // 初始化字符设备
    cdev_init(&ascii_device.cdev, &fops);
    ascii_device.cdev.owner = THIS_MODULE;

    // 添加字符设备
    ret = cdev_add(&ascii_device.cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        printk(KERN_ALERT "ascii_char_device: Failed to add cdev\n");
        return ret;
    }

    // 初始化信号量和等待队列
    sema_init(&ascii_device.sem, 1);
    init_waitqueue_head(&ascii_device.read_queue);
    init_waitqueue_head(&ascii_device.write_queue);

    printk(KERN_INFO "ascii_char_device: Device initialized successfully\n");
    return 0;
}

// 模块清理函数
static void __exit ascii_exit(void) {
    dev_t dev = MKDEV(major_number, 0);
    struct message *msg, *tmp;

    // 删除所有消息
    list_for_each_entry_safe(msg, tmp, &ascii_device.msg_list, list) {
        list_del(&msg->list);
        kfree(msg);
    }

    // 删除字符设备
    cdev_del(&ascii_device.cdev);

    // 注销设备号
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "ascii_char_device: Device unregistered and cleaned up\n");
}

module_init(ascii_init);
module_exit(ascii_exit);
