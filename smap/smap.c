/*
 * SMAP is a module to test the Supervisor Mode Access Prevention feature.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include "smap.h"

static int value;

static long smap_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch(cmd) {

	case SMAP_IOC_READ:
		pr_debug("SMAP_IOC_READ: arg=0x%lx\n", arg);
		ret = put_user(value, (int __user *)arg);
		break;

	case SMAP_IOC_WRITE:
		pr_debug("SMAP_IOC_WRITE: arg=0x%lx\n", arg);
		ret = get_user(value, (int __user *)arg);
		break;

	case SMAP_IOC_DIRECT_READ:
		pr_debug("SMAP_IOC_DIRECT_READ: arg=0x%lx\n", arg);
		*(int *)arg = value;
		break;

	case SMAP_IOC_DIRECT_WRITE:
		pr_debug("SMAP_IOC_DIRECT_WRITE: arg=0x%lx\n", arg);
		value = *(int *)arg;
		break;

	default:
		ret = -ENOTTY;

	}

	return ret;
}

static const struct file_operations smap_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = smap_ioctl,
};

static struct miscdevice smap_device = {
	.name = THIS_MODULE->name,
	.fops = &smap_fops,
};

static int smap_init(void)
{
	return misc_register(&smap_device);
}

static void smap_exit(void)
{
	misc_deregister(&smap_device);
}

module_init(smap_init);
module_exit(smap_exit);
