/*
 * uam just returns the value of user_addr_max().
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static ssize_t uam_read(struct file *filp, char __user *ubuf,
			size_t count, loff_t *offp)
{
	ssize_t size;
	char kbuf[128];
	size = scnprintf(kbuf, 128, "user_addr_max=0x%lx\n",  user_addr_max());
	return simple_read_from_buffer(ubuf, count, offp, kbuf, size);
}

static const struct file_operations uam_fops = {
	.owner = THIS_MODULE,
	.read = uam_read,
};

static struct miscdevice uam_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = THIS_MODULE->name,
	.fops = &uam_fops,
};

static int __init uam_init(void)
{
	return misc_register(&uam_device);
}

static void __exit uam_exit(void)
{
	misc_deregister(&uam_device);
}


module_init(uam_init);
module_exit(uam_exit);
