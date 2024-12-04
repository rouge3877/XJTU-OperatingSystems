#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>

#define sys_No 96

unsigned long * p_sys_call_table;
unsigned long old_sys_call_func;

asmlinkage long hello(const struct pt_regs *regs)
{
    printk(KERN_INFO "No 96 syscall has changed to hello");
    return regs->di + regs->si;
}

void disable_write_protection(void)
{
    unsigned long cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~0x00010000;
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void enable_write_protection(void)
{
    unsigned long cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x00010000;
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void modify_syscall(void)
{
    printk(KERN_INFO "Modifying syscall table\n");
    printk(KERN_INFO "sys_call_table address: %px\n", p_sys_call_table);
    printk(KERN_INFO "sys_call_table[96] address: %px\n", &(p_sys_call_table[8 * sys_No]));

    old_sys_call_func = p_sys_call_table[8 * sys_No];
    p_sys_call_table[8 * sys_No] = (unsigned long)hello;
}

void restore_syscall(void)
{
    p_sys_call_table[8 * sys_No] = old_sys_call_func;
}

static int __init mymodule_init(void)
{
    p_sys_call_table = (unsigned long *)0xffffffffa5600320;
    if (!p_sys_call_table) {
        printk(KERN_ERR "Failed to find sys_call_table\n");
        return -1;
    }

    disable_write_protection();
    modify_syscall();
    enable_write_protection();

    printk(KERN_INFO "Module loaded successfully.\n");
    return 0;
}

static void __exit mymodule_exit(void)
{
    disable_write_protection();
    restore_syscall();
    enable_write_protection();

    printk(KERN_INFO "Module unloaded successfully.\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
