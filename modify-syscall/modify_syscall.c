#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// original, syscall 78 function: gettimeofday
// new syscall 78 function: print "No 78 syscall has changed to hello" and return a+b
#define sys_No 78

unsigned long old_sys_call_func;
unsigned long p_sys_call_table = 0xffffffffffffffff ; // find in /boot/System.map-$(uname -r)

// New system call function
asmlinkage int hello(int a, int b) 
{
    printk("No 78 syscall has changed to hello");
    return a + b; // Return sum of a and b
}

// Modify system call table
void modify_syscall(void) 
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * 4);  // Calculate the address of syscall 78
    old_sys_call_func = *(sys_call_addr);  // Save the original syscall address
    *(sys_call_addr) = (unsigned long)&hello;  // Point to the new function (hello)
}

// Restore original system call
void restore_syscall(void)
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * 4);  // Same address calculation
    *(sys_call_addr) = old_sys_call_func;  // Restore the original syscall address
}

// Module initialization function
static int mymodule_init(void)
{
    modify_syscall();  // Modify syscall table on module load
    return 0;
}

// Module exit function
static void mymodule_exit(void)
{
    restore_syscall();  // Restore original syscall on module unload
}

module_init(mymodule_init);  // Register init function
module_exit(mymodule_exit);  // Register exit function
MODULE_LICENSE("GPL");  // Specify GPL license for the module

