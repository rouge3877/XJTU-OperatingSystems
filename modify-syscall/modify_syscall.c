#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

// 用 sudo cat /proc/kallsyms | grep sys_call_table 获取 sys_call_table 的内核地址
// 注意：每次重启内核必须再次获取地址
#define SYS_CALL_TABLE_ADDRESS 0xffffffff82200320
// 指向系统调用数组
unsigned long *sys_call_table = (unsigned long *)SYS_CALL_TABLE_ADDRESS;

// 保存原始的系统调用
int (*original_gettimeofday)(struct timeval *tv, struct timezone *tz);

// 用于操作CR0寄存器的函数
unsigned int ClearAndReturnCr0(void);
void SetbackCr0(unsigned int val);

// 新的系统调用函数
// syscall 78 :gettimeofday
// format: int gettimeofday(struct timeval *tv, struct timezone *tz);
// tv for time, tz for timezone
// tv is a struct timeval {time_t tv_sec; suseconds_t tv_usec;}
// tz is a struct timezone {int tz_minuteswest; int tz_dsttime;}
// sizeof(struct timeval) = 16, sizeof(struct timezone) = 8
// in new syscall, we set time as ZERO
asmlinkage int MyGettimeofday(struct timeval *tv, struct timezone *tz) {
    printk(KERN_INFO "No 78 syscall has changed to hello\n");

    printk(KERN_INFO "tv: %p\n", tv);
    printk(KERN_INFO "tz: %p\n", tz);

    printk(KERN_INFO "tv->tv_sec: %ld\n", tv->tv_sec);
    printk(KERN_INFO "tv->tv_usec: %ld\n", tv->tv_usec);
    printk(KERN_INFO "tz->tz_minuteswest: %d\n", tz->tz_minuteswest);
    printk(KERN_INFO "tz->tz_dsttime: %d\n", tz->tz_dsttime);

    tv->tv_sec = 0;
    tv->tv_usec = 0;
    return tv->tv_sec + tv->tv_usec;
}

// 写保护禁用与恢复：为了修改只读的系统调用表，需要暂时禁用内存写保护。
// 这通过修改CR0控制寄存器实现，CR0的某一位控制着CPU是否允许对只读页面的写操作。
// 关闭CR0寄存器的写保护位
unsigned int ClearAndReturnCr0(void) {
    unsigned int cr0 = 0;
    unsigned int ret;

    asm volatile ("movq %%cr0, %%rax" : "=a"(cr0));
    ret = cr0;

    // 清除写保护位
    cr0 &= 0xfffeffff;
    asm volatile ("movq %%rax, %%cr0" :: "a"(cr0));

    return ret;
}

// 恢复CR0寄存器的原始值
void SetbackCr0(unsigned int val) {
    asm volatile ("movq %%rax, %%cr0" : : "a"(val));
}

// 模块加载时的初始化函数
static int __init ModuleInit(void) {
    printk(KERN_INFO "Module is being loaded.\n");

    // 清除CR0寄存器的写保护位
    unsigned int orig_cr0 = ClearAndReturnCr0();

    // 替换getdents系统调用
    original_gettimeofday = (int (*)(struct timeval *tv, struct timezone *tz))sys_call_table[__NR_gettimeofday];
    sys_call_table[__NR_gettimeofday] = (unsigned long)MyGettimeofday;

    // 恢复CR0的原始值
    SetbackCr0(orig_cr0);

    return 0;
}

// 模块卸载时的清理函数
static void __exit ModuleExit(void) {
    printk(KERN_INFO "Module is being unloaded.\n");

    // 清除CR0寄存器的写保护位
    unsigned int orig_cr0 = ClearAndReturnCr0();

    // 恢复原始的getdents系统调用
    sys_call_table[__NR_gettimeofday] = (unsigned long)original_gettimeofday;

    // 恢复CR0的原始值
    SetbackCr0(orig_cr0);
}

module_init(ModuleInit);
module_exit(ModuleExit);