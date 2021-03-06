/* A kernel module providing a character device that is the binary
   opposite of zero. */

#include <linux/module.h>    /* included for all kernel modules */
#include <linux/kernel.h>    /* included for KERN_INFO */
#include <linux/init.h>      /* included for __init and __exit macros */
#include <linux/device.h>
#include <linux/fs.h>

#define DEVICE_NAME "one"
#define CLASS_NAME "one"

/* Prototypes */
static void one_destroy(void);
ssize_t one_file_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t one_file_write(struct file *filp, const char __user * buf, size_t count, loff_t *f_pos);


/* Device variables */
static int one_major = -1;
static struct class* one_class = NULL;
static struct device* one_device = NULL;

static unsigned short output_value = 0xFF;
module_param_named(outval, output_value, ushort, S_IRUGO);
MODULE_PARM_DESC(outval, "Overrides the default output value (0xFF).");


struct file_operations one_fops = {
    .owner = THIS_MODULE,
    .open = NULL,
    .release = NULL,
    .read = one_file_read,
    .write = one_file_write,
    .llseek = NULL,
    .mmap = NULL,
};


static int __init one_init(void)
{
    int ret = 0;

    printk(KERN_INFO "one: Module initialization!");

    /* First, see if we can dynamically allocate a major for our device */
    one_major = register_chrdev(0, DEVICE_NAME, &one_fops);
    if (one_major < 0) {
        printk(KERN_ERR "one: failed to register device: error %d.\n", one_major);
        ret = one_major;
        goto cleanup;
    }

    one_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(one_class))
    {
        printk(KERN_ERR "one: failed to register device class '%s'.\n", CLASS_NAME);
        ret = PTR_ERR(one_class);
        goto cleanup;
    }

    /* With a class, the easiest way to instantiate a device is to call device_create() */
    one_device = device_create(one_class,
                               NULL,
                               MKDEV(one_major, 0),
                               NULL,
                               DEVICE_NAME);
    if (IS_ERR(one_device))
    {
        printk(KERN_ERR "one: failed to create device '%s'.\n", DEVICE_NAME);
        ret = PTR_ERR(one_device);
        goto cleanup;
    }

cleanup:
    if (0 != ret)
    {
        one_destroy();
    }

    return ret;
}


static void __exit one_exit(void)
{
    printk(KERN_INFO "one: Module clean-up and exit!");

    one_destroy();
}


static void one_destroy(void)
{
    printk(KERN_INFO "one: Unregistering device.\n");

    if (one_device > 0) device_destroy(one_class, MKDEV(one_major, 0));

    if (one_class > 0)
    {
        class_unregister(one_class);
        class_destroy(one_class);
    }

    if (one_major > 0) unregister_chrdev(one_major, DEVICE_NAME);
}


ssize_t one_file_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    memset(buf, (unsigned char)output_value, count);

    return count;
}

ssize_t one_file_write(struct file *filp, const char __user * buf, size_t count, loff_t *f_pos)
{
    int ret = 0;
    long value = -1;

    ret = kstrtol_from_user(buf, count, 10, &value);

    if (ret == 0)
    {
        output_value = (unsigned short)value + 1;
    }

    return count;
}


module_init(one_init);
module_exit(one_exit);

MODULE_AUTHOR("Jean-Sebastien A. Beaudry");
MODULE_DESCRIPTION("Not /dev/zero");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
