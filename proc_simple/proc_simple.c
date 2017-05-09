#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

static char *proc_simple_filename = "simple";
static struct proc_dir_entry *proc_simple_entry;

ssize_t proc_simple_read(struct file *filp, char __user *u_buff, size_t count,
			loff_t *offp)
{
	return simple_read_from_buffer(u_buff, count, offp,
		proc_simple_filename, strlen(proc_simple_filename));
}

static const struct file_operations proc_simple_fops = {
	.read = proc_simple_read,
};

static __init int proc_simple_init(void)
{
	proc_simple_entry = proc_create(proc_simple_filename, 0444, NULL,
					&proc_simple_fops);

	if (proc_simple_entry == NULL) {
		pr_debug("Unable to create proc '%s'\n", proc_simple_filename);
		return -ENOMEM;
	}

	return 0;
}

static __exit void proc_simple_exit(void)
{
	proc_remove(proc_simple_entry);
}

module_init(proc_simple_init);
module_exit(proc_simple_exit);

MODULE_LICENSE("GPL");
