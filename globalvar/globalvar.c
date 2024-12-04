#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>                                                        
#include <asm/uaccess.h>  
#include <linux/init.h>

// 模块许可证
MODULE_LICENSE("GPL");                                                       

// 定义字符设备的主设备号
#define MAJOR_NUM 290                                                     

// 读取操作
static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off);

// 写入操作
static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off);

// 定义文件操作结构体，并将其与实际的函数绑定
struct file_operations globalvar_fops = {                                     
    .read = globalvar_read,  // 绑定读取函数
    .write = globalvar_write  // 绑定写入函数
};

// 全局变量，存储设备的值
static int global_var = 0; 

// 模块初始化函数，注册字符设备
static int init_mymodule(void) {
    int ret;
    
    // 注册字符设备，返回值为负数时表示注册失败
    ret = register_chrdev(MAJOR_NUM, "globalvar", &globalvar_fops);
    if (ret) {
        printk(KERN_ERR "globalvar register failure\n");
    } else {
        printk(KERN_INFO "globalvar register success\n");
    }
    
    return ret;
}  

// 模块退出函数，注销字符设备
static void cleanup_mymodule(void) {
    unregister_chrdev(MAJOR_NUM, "globalvar"); 
}

// 读取函数，将内核中的全局变量复制到用户空间
static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off) {
    // 将内核变量 global_var 的值复制到用户空间
    if (copy_to_user(buf, &global_var, sizeof(int))) {
        return -EFAULT;  // 如果复制失败，返回 EFAULT 错误
    }
    
    return sizeof(int);  // 返回成功读取的字节数
}

// 写入函数，将用户空间的数据复制到内核中的全局变量
static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off) {
    // 将用户传入的数据复制到内核变量 global_var
    if (copy_from_user(&global_var, buf, sizeof(int))) {
        return -EFAULT;  // 如果复制失败，返回 EFAULT 错误
    }
    
    return sizeof(int);  // 返回成功写入的字节数
}

// 注册模块初始化和退出函数
module_init(init_mymodule);
module_exit(cleanup_mymodule);

