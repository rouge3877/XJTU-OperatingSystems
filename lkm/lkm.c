// lkm.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int lkm_init(void)
{
    printk("bt: module loaded\n");
    return 0;
}

static void lkm_exit(void)
{
    printk("bt: module removed\n");
}

module_init(lkm_init);
module_exit(lkm_exit);