#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>

static struct task_struct *thread1;
static struct task_struct *thread2;
static struct mutex first_mutex;
static struct mutex second_mutex;

static int thread_function1(void *data)
{
    mutex_lock(&first_mutex);
    pr_info("Thread ONE acquired first_mutex\n");
    msleep(1000);
    mutex_lock(&second_mutex);
    pr_info("Thread ONE acquired second_mutex\n");
    mutex_unlock(&second_mutex);
    pr_info("Thread ONE released second_mutex\n");
    mutex_unlock(&first_mutex);
    pr_info("Thread ONE released first_mutex\n");
    do_exit(0);
}

static int thread_function2(void *data)
{
    mutex_lock(&second_mutex);
    pr_info("Thread TWO acquired second_mutex\n");
    msleep(1000);
    mutex_lock(&first_mutex);
    pr_info("Thread TWO acquired first_mutex\n");
    mutex_unlock(&first_mutex);
    pr_info("Thread TWO released first_mutex\n");
    mutex_unlock(&second_mutex);
    pr_info("Thread TWO released second_mutex\n");
    do_exit(0);
}

static int __init my_module_init(void)
{
    mutex_init(&first_mutex);
    mutex_init(&second_mutex);

    thread1 = kthread_run(thread_function1, NULL, "thread1");
    if (IS_ERR(thread1)) {
        pr_err("Failed to create thread1\n");
        mutex_destroy(&first_mutex);
        mutex_destroy(&second_mutex);
        return PTR_ERR(thread1);
    }

    thread2 = kthread_run(thread_function2, NULL, "thread2");
    if (IS_ERR(thread2)) {
        pr_err("Failed to create thread2\n");
        kthread_stop(thread1);
        mutex_destroy(&first_mutex);
        mutex_destroy(&second_mutex);
        return PTR_ERR(thread2);
    }

    pr_info("Threads created\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    kthread_stop(thread1);
    kthread_stop(thread2);
    mutex_destroy(&first_mutex);
    mutex_destroy(&second_mutex);
    pr_info("Threads stopped\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example kernel module");