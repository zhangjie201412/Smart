#include <linux/slab.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/fs.h> 
#include <linux/platform_device.h>
#include <linux/device.h>
#include <mach/gpio.h> 

#define RF_RESET_PIN        NUC970_PG5

#define SIM800_PWR_EN       NUC970_PD14
#define SIM800_PWR_KEY      NUC970_PD12

static int gprs_powerup(void)
{
    pr_info("%s: E\n", __func__);
    gpio_set_value(SIM800_PWR_EN, 1);
    gpio_set_value(SIM800_PWR_KEY, 1);
    msleep(1000);
    gpio_set_value(SIM800_PWR_KEY, 0);
    pr_info("%s: X\n", __func__);

    return 0;
}

static void gprs_powerdown(void)
{
    pr_info("%s: E\n", __func__);
    gpio_set_value(SIM800_PWR_EN, 0);
    pr_info("%s: X\n", __func__);
}

static int led_init(void)
{
    int ret;

    ret = gpio_request(RF_RESET_PIN, "rf_reset_pin");
    if(ret) {
        pr_err("%s: failed to request rf reset pin\n", __func__);
        return -1;
    }
    ret = gpio_request(SIM800_PWR_EN, "sim800_pwren_pin");
    if(ret) {
        pr_err("%s: failed to request sim800 poweren pin\n", __func__);
        return -1;
    }
    ret = gpio_request(SIM800_PWR_KEY, "sim800_pwrkey_pin");
    if(ret) {
        pr_err("%s: failed to request sim800 powerkey pin\n", __func__);
        return -1;
    }
    gpio_direction_output(RF_RESET_PIN, 1);
    gpio_direction_output(SIM800_PWR_EN, 0);
    gpio_direction_output(SIM800_PWR_KEY, 0);
    return 0;
}

static void gprs_free(void)
{
    gpio_free(RF_RESET_PIN);
    gpio_free(SIM800_PWR_EN);
    gpio_free(SIM800_PWR_KEY);
}


static ssize_t gprs_power_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{

    return 0;
}

static ssize_t gprs_power_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    int value;

    sscanf(buf, "%d", &value);
    if(value) {
        gprs_powerup();
    } else {
        gprs_powerdown();
    }

    return count;
}

static ssize_t gprs_led_status_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t gprs_led_status_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    int value;

    sscanf(buf, "%d", &value);

    return count;
}


static ssize_t gprs_led_ppp_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t gprs_led_ppp_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    int value;

    sscanf(buf, "%d", &value);

    return count;
}

static ssize_t gprs_led_lan_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t gprs_led_lan_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    int value;

    sscanf(buf, "%d", &value);

    return count;
}

static ssize_t gprs_lan_power_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t gprs_lan_power_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    int value;

    gpio_set_value(RF_RESET_PIN, 1);
    msleep(100);
    gpio_set_value(RF_RESET_PIN, 0);
    msleep(100);
    printk("%s: rf reset done\n", __func__);

    return count;
}

static DEVICE_ATTR(gprs_power, S_IWUSR | S_IRUGO,
        gprs_power_show,
        gprs_power_store);
static DEVICE_ATTR(led_status, S_IWUSR | S_IRUGO,
        gprs_led_status_show,
        gprs_led_status_store);
static DEVICE_ATTR(led_ppp, S_IWUSR | S_IRUGO,
        gprs_led_ppp_show,
        gprs_led_ppp_store);
static DEVICE_ATTR(led_lan, S_IWUSR | S_IRUGO,
        gprs_led_lan_show,
        gprs_led_lan_store);

static DEVICE_ATTR(lan_power, S_IWUSR | S_IRUGO,
        gprs_lan_power_show,
        gprs_lan_power_store);

static struct attribute *led_attrs[] = {
    &dev_attr_gprs_power.attr,
    &dev_attr_led_status.attr,
    &dev_attr_led_ppp.attr,
    &dev_attr_led_lan.attr,
    &dev_attr_lan_power.attr,
    NULL,
};

static struct attribute_group led_attr_group = {
    .attrs = led_attrs,
};

static int gprs_power_probe(struct platform_device *pdev)
{
    int ret;

    pr_info("%s\n", __func__);
    ret = led_init();
    if(ret < 0) {
        pr_err("%s: failed to init led pins\n", __func__);
    } else {
        //create led sys files
        ret = sysfs_create_group(&pdev->dev.kobj, &led_attr_group);
        if (ret) {
            pr_err("%s: Unable to export led, error: %d\n", __func__,
                    ret);
            return -1;
        }
    }
    return 0;
}

static int gprs_power_remove(struct platform_device *pdev)
{
    pr_info("%s\n", __func__);
	sysfs_remove_group(&pdev->dev.kobj, &led_attr_group);
    gprs_free();
    return 0;
}

static struct platform_driver gprs_power_driver = {
    .driver		= {
        .owner	= THIS_MODULE,
        .name	= "gprs_power",
    },
    .probe		= gprs_power_probe,
    .remove		= gprs_power_remove,
    .suspend	= NULL,
    .resume		= NULL,
};

static int __init gprs_power_init(void)
{
    return platform_driver_register(&gprs_power_driver);
}

static void __exit gprs_power_exit(void)
{
    platform_driver_unregister(&gprs_power_driver);
}

MODULE_DESCRIPTION("gprs power init driver");
MODULE_LICENSE("GPLv3");
MODULE_VERSION("0.2");

module_init(gprs_power_init);
module_exit(gprs_power_exit);

