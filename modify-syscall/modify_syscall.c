#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
// original,syscall 78 function: gettimeofday
//  new syscall 78 function: print "No 78 syscall has changed to hello" and return a+b
#define sys_No 78

unsigned long old_sys_call_func;
unsigned long p_sys_call_table = 0xffffffffc16b8000; // find in  /boot/System.map-'uname -r'

asmlinkage int hello(long long int a, long long int b) // new function
{
    printk("No 78  syscall has changed to hello");
    return a + b;
}
void modify_syscall(void)
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * 4);
    old_sys_call_func = *(sys_call_addr);
    *(sys_call_addr) = (unsigned long)&hello; //  point to new function
}
void restore_syscall(void)
{
    unsigned long *sys_call_addr;
    sys_call_addr = (unsigned long *)(p_sys_call_table + sys_No * 4);
    *(sys_call_addr) = old_sys_call_func; // point to original function
}
static int mymodule_init(void)
{
    modify_syscall();
    return 0;
}
static void mymodule_exit(void)
{
    restore_syscall();
}
module_init(mymodule_init);
module_exit(mymodule_exit);
MODULE_LICENSE("GPL");
