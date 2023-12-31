#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/syscalls.h>
#include <linux/list.h> 
#include <linux/signal.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya and Sahil");
MODULE_DESCRIPTION("module counting number of running processes.");
#define __NR_count_processes 1000
asmlinkage long counting_process(void) {
    struct task_struct *task;
    long count = 0;
    for_each_process(task) {
       // printk(KERN_INFO "%d",task->__state);
        // task->__state == 0
        if (task_is_running(task)){ 
            //printk(task->comm);
            count++;    
       // printk("HI\n");
        }
    }
    return count;
}
static int __init display_init(void) {
    long k;
    printk(KERN_INFO "Module loaded ... \n");
    k=counting_process();
    printk(KERN_INFO "The number of running processes are: %ld\n",k);
    return 0;
}
static void __exit display_cleanup(void) {
    printk(KERN_INFO "Module Unloaded ... \n");
    // Unregister the custom system call
    // sys_call_table[__NR_count_processes] = NULL;
}
module_init(display_init);
module_exit(display_cleanup);