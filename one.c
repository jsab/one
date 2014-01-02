/* A kernel module providing a character device that is the binary
   opposite of zero. */

#include <linux/module.h>    /* included for all kernel modules */
#include <linux/kernel.h>    /* included for KERN_INFO */
#include <linux/init.h>      /* included for __init and __exit macros */


static int __init one_init(void)
{
    printk(KERN_INFO "one: Module initialization!");
    return 0;    /* Zero = success */
}


static void __exit one_exit(void)
{
    printk(KERN_INFO "one: Module clean-up and exit!");
}


module_init(one_init);
module_exit(one_exit);

MODULE_AUTHOR("Jean-Sebastien A. Beaudry");
MODULE_DESCRIPTION("Not /dev/zero");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
