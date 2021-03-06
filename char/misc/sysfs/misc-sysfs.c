#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h> //for copy_to/from_user
#include <linux/slab.h> //for kmalloc/kfree
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

//This example shows how to create a sysfs attibute. We are just interested in single entry point.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amitesh Singh");
MODULE_DESCRIPTION("misc device");
MODULE_VERSION("0.1"); // --> module version of kernel

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release
};

static int
dev_open(struct inode *in, struct file *fl)
{
   printk(KERN_INFO "device is opened\n");

   return 0;
}

static int
dev_release(struct inode *in, struct file *fl)
{
   printk(KERN_INFO "device is closed, applied close(fd)?\n");

   return 0;
}

static ssize_t
dev_read(struct file *f, char *buf, size_t len, loff_t *offset)
{
   return len;
}

static ssize_t
dev_write(struct file *f, const char *buf, size_t len, loff_t *offset)
{
   printk(KERN_INFO "This all goes to black hole now.\n");

   return len;
}

static struct miscdevice my_misc_device = {
    .minor = 255, //let kernel decide, or MISC_DYNAMIC_MINOR
    .name = "misc-sysfs",  //sysfs entry -> /sys/class/misc/my_misc_device, /dev/my_misc_device
    .fops = &fops,
};

static long old_value;

static ssize_t get_value(struct device *dev,
                         struct device_attribute *attr, char *buf)
{
   return sprintf(buf, "%ld", old_value);
}

static ssize_t set_value(struct device *dev,
                 struct device_attribute *attr,
                 const char *buf, size_t count)
{
   long new_value = 0;

   if (kstrtol(buf, 10, &new_value) < 0)
     {
        return -EINVAL;
     }

   old_value = new_value;

   return count;
}

static DEVICE_ATTR(value, 0665, get_value,
                   set_value);

static int __init
misc_init(void)
{
   printk(KERN_INFO "misc device init");
   if (misc_register(&my_misc_device))
   {
     printk(KERN_ALERT "Failed to create misc device");
     return -ENOMEM;
   }

   device_create_file(my_misc_device.this_device, &dev_attr_value);

   return 0;
}

static void __exit
misc_exit(void)
{
  printk(KERN_INFO "misc device exit");
  device_remove_file(my_misc_device.this_device, &dev_attr_value);

  misc_deregister(&my_misc_device);
}

module_init(misc_init);
module_exit(misc_exit);
