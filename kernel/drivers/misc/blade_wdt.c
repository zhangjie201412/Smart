/*
 * Copyright (C) 2016-2017 zhbsh.zhbsh@gmail.com, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/sched.h>   //wake_up_process()  
#include <linux/kthread.h> //kthread_create()?°Èkthread_run()  
#include <linux/err.h> //IS_ERR()?°ÈPTR_ERR() 
#include <linux/fs.h> 
#include <linux/platform_device.h>
#include <linux/device.h>
#include <mach/gpio.h> 

#define HW_WATCHDOG_GPIO NUC970_PJ0
#define HW_OUTPUT_GPIO NUC970_PJ1
#define AUDIO_AMPLIFY_SHUTDOWN NUC970_PD15
#define AUDIO_AUDIO_EN NUC970_PD11

#define HW_WATCHDOG_MAGIC		0xE9
/*
 * HW_WATCHDOG_SET_VALUE set level via ioctl
 * HW_WATCHDOG_SET_VALUE(0): pull down
 * HW_WATCHDOG_SET_VALUE(1): pull high
 */
#define HW_WATCHDOG_SET_VALUE	_IOW(HW_WATCHDOG_MAGIC, 0x01, unsigned int)
/*
 * HW_WATCHDOG_ENABLE set level via ioctl
 * HW_WATCHDOG_ENABLE(0): disable hw watchdog in kernel
 * HW_WATCHDOG_ENABLE(1): enable hw watchdog in kernel
 */
#define HW_WATCHDOG_ENABLE		_IOW(HW_WATCHDOG_MAGIC, 0x02, unsigned int)

/* major number of device */
static int gMajor;
static struct class *watchdog_class;

#ifdef WATCHDOG_DELAYED_WORK
static struct delayed_work *delay_work;
#else
static wait_queue_head_t read_wq;
static struct task_struct *read_task;
static bool running = true;
#endif
static bool flag = true;

#ifdef CONFIG_BOARD_NUC970_LP
static void pins_control_init() {
    //printk("Board LP IO Init\n");
    gpio_request(NUC970_PF10, "FEC_RST");
    gpio_direction_output(NUC970_PF10, 0);
    msleep(10);
    gpio_direction_output(NUC970_PF10, 1);
    gpio_free(NUC970_PF10);
}

static void pins_control_deinit() {
    gpio_free(NUC970_PF10);

}
#elif CONFIG_BOARD_NUC970_IC
static void pins_control_init() {
    gpio_request(AUDIO_AMPLIFY_SHUTDOWN, "audio_ampify_shutdown");
    gpio_direction_output(AUDIO_AMPLIFY_SHUTDOWN, 1);

    gpio_request(AUDIO_AUDIO_EN, "audio_en");
    gpio_direction_output(AUDIO_AUDIO_EN, 1);
}

static void pins_control_deinit() {
    gpio_free(AUDIO_AMPLIFY_SHUTDOWN);
    gpio_free(AUDIO_AUDIO_EN);
}
#else
static void pins_control_init() {
    gpio_request(AUDIO_AMPLIFY_SHUTDOWN, "audio_ampify_shutdown");
    gpio_direction_output(AUDIO_AMPLIFY_SHUTDOWN, 1);

    gpio_request(AUDIO_AUDIO_EN, "audio_en");
    gpio_direction_output(AUDIO_AUDIO_EN, 1);
}

static void pins_control_deinit() {
    gpio_free(AUDIO_AMPLIFY_SHUTDOWN);
    gpio_free(AUDIO_AUDIO_EN);
}

#endif

#ifdef WATCHDOG_DELAYED_WORK
void timer_caller(struct work_struct *work)
{
    //printk("feed mcu wdog\n");
    if(flag) {
        gpio_set_value(HW_WATCHDOG_GPIO , 1);
        gpio_set_value(HW_OUTPUT_GPIO , 1);
    }
    //msleep(500);
    else {
        gpio_set_value(HW_WATCHDOG_GPIO, 0);
        gpio_set_value(HW_OUTPUT_GPIO, 0);
    }

    schedule_delayed_work(to_delayed_work(work), 0.5*HZ);
    flag = !flag;
}
#else
int watchdog_thread(void *data)
{
    int ret = 0;

    while(running) {
        ret = wait_event_interruptible_timeout(read_wq, !running, 0.5*HZ);
        if(flag) {
            gpio_set_value(HW_WATCHDOG_GPIO , 1);
            gpio_set_value(HW_OUTPUT_GPIO , 1);
        } else {
            gpio_set_value(HW_WATCHDOG_GPIO, 0);
            gpio_set_value(HW_OUTPUT_GPIO, 0);
        }

        flag = !flag;
    }

    return ret;
}
#endif

static int hw_watchdog_open(struct inode* inode, struct file* filp) {
    //printk(KERN_ERR "hw_watchdog_open\n");
    return 0;
}  

static int hw_watchdog_close(struct inode* inode, struct file* filp) {
    //printk(KERN_ERR "hw_watchdog_close\n");
    return 0;  
}  

static long hw_watchdog_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

    long ret = 0;

    switch (cmd) {	
        case HW_WATCHDOG_SET_VALUE:
            if (arg == 1) {
                /* pull high */
                //printk("%s pull high\n", __func__);
                gpio_set_value(HW_WATCHDOG_GPIO , 1);
                gpio_set_value(HW_OUTPUT_GPIO , 1);
            } else  if (arg == 0) {
                /* pull down */
                //printk("%s pull down\n", __func__);
                gpio_set_value(HW_WATCHDOG_GPIO , 0);
                gpio_set_value(HW_OUTPUT_GPIO , 0);

            } else {
                printk("%s bad arg %d\n", __func__, (int)(arg));
                return -EINVAL;
            }
            break;

        case HW_WATCHDOG_ENABLE:
            if (arg == 1) {
                /* enable */
                printk("%s enable hw watchdog, can't support\n", __func__);

            } else  if (arg == 0) {
                /* pull down */
                printk("%s disable hw watchdog\n", __func__);
                /* Wake up waiting readers */
                running = false;
                wake_up(&read_wq);	
                kthread_stop(read_task);			
            } else {
                printk("%s bad arg %d\n", __func__, (int)(arg));
                return -EINVAL;
            }
            break;

        default:
            printk("%s bad ioctl %d\n", __func__, cmd);
            return -EINVAL;
    }

    return ret;
}

struct file_operations hw_watchdog_fops = {
    .owner =    THIS_MODULE,
    .llseek =   NULL,
    .read =     NULL,
    .write =    NULL,
    .unlocked_ioctl = hw_watchdog_ioctl,
    .compat_ioctl = hw_watchdog_ioctl,
    .open =     hw_watchdog_open,
    .release =  hw_watchdog_close,
};

static int hw_watchdog_probe(struct platform_device *pdev) {	
    struct device *temp_class;
    int M;
    int ret = -1;

    printk(KERN_WARNING "hw_watchdog probe\n");
    /* register a character device */
    M = register_chrdev(0, "hw_watchdog", &hw_watchdog_fops);
    if (M < 0) {
        printk("\nhw_watchdog: can't get major number\n");
        return M;
    }
    gMajor = M;

    watchdog_class = class_create(THIS_MODULE, "hw_watchdog");
    if (IS_ERR(watchdog_class)) {
        printk(KERN_ERR "Error creating hw_watchdog module class.\n");
        unregister_chrdev(gMajor, "hw_watchdog");
        return PTR_ERR(watchdog_class);
    }

    temp_class = device_create(watchdog_class, NULL, MKDEV(gMajor, 0), NULL, "hw_watchdog");
    if (IS_ERR(temp_class)) {
        printk(KERN_ERR "Error creating hw_watchdog class device.\n");
        class_destroy(watchdog_class);
        unregister_chrdev(gMajor, "hw_watchdog");
        return -1;
    }

    ret = gpio_request(HW_WATCHDOG_GPIO, "hw_watchdog_pin");
    if(ret==0) {
        gpio_direction_output(HW_WATCHDOG_GPIO, 0);
        gpio_set_value(HW_WATCHDOG_GPIO , 1);
        msleep(2);
        gpio_set_value(HW_WATCHDOG_GPIO , 0);
    } else {
        printk("hw watchdog gpio is occupied already\n");
    }

    ret = gpio_request(HW_OUTPUT_GPIO, "hw_output_pin");
    if(ret==0) {
        gpio_direction_output(HW_OUTPUT_GPIO, 0);
        gpio_set_value(HW_OUTPUT_GPIO , 1);
        msleep(2);
        gpio_set_value(HW_OUTPUT_GPIO , 0);
    } else {
        printk("hw output gpio is occupied already\n");
    }

    pins_control_init();

#ifdef WATCHDOG_DELAYED_WORK
    delay_work = kmalloc(sizeof(*delay_work), GFP_KERNEL);
    INIT_DELAYED_WORK(delay_work, timer_caller);
    schedule_delayed_work(delay_work, 0.5*HZ);
#else
    /* init mutex and queues */
    init_waitqueue_head(&read_wq);
    read_task = kthread_create(watchdog_thread, NULL, "watchdog_thread");
    if(!IS_ERR(read_task)) {
        wake_up_process(read_task);
    }
#endif

    return 0;
}

static int hw_watchdog_remove(struct platform_device *pdev) {
    pins_control_deinit();
#ifdef WATCHDOG_DELAYED_WORK
    cancel_delayed_work_sync(delay_work);
    kfree(delay_work);
#else
    /* Wake up waiting readers */
    running = false;
    wake_up(&read_wq);	
    kthread_stop(read_task);
#endif	
    gpio_free(HW_WATCHDOG_GPIO);
    gpio_free(HW_OUTPUT_GPIO);

    printk(KERN_WARNING "hw_watchdog remove");
    unregister_chrdev(gMajor, "hw_watchdog");
    device_destroy(watchdog_class, MKDEV(gMajor, 0));
    class_destroy(watchdog_class);

    return 0;
}

static struct platform_driver hw_watchdog_driver = {
    .driver		= {
        .owner	= THIS_MODULE,
        .name	= "blade-watchdog",
    },
    .probe		= hw_watchdog_probe,
    .remove		= hw_watchdog_remove,
    .suspend	= NULL,
    .resume		= NULL,
};

static int __init hw_watchdog_init(void)
{
    return platform_driver_register(&hw_watchdog_driver);
}

static void __exit hw_watchdog_exit(void)
{
    platform_driver_unregister(&hw_watchdog_driver);
}

MODULE_DESCRIPTION("blade watchdog driver");
MODULE_LICENSE("GPLv3");
MODULE_VERSION("0.2");

module_init(hw_watchdog_init);
module_exit(hw_watchdog_exit);
