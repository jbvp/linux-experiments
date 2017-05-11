/*
 * proc_task lists the tasks (pid + comm) in /proc/tasks
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>

static const char *pt_filename = "tasks";
static struct proc_dir_entry *pt_entry;

ssize_t pt_read(struct file *filp, char __user *u_buff, size_t count,
			loff_t *offp)
{
	struct task_struct *t;
	char *k_buff;
	size_t size = PAGE_SIZE;
	size_t len;
	size_t len_sum = 0;
	ssize_t retval;

	k_buff = kzalloc(size, GFP_KERNEL);
	if (!k_buff)
		return -ENOMEM;

	list_for_each_entry(t, &init_task.tasks, tasks) {
		len = scnprintf(&k_buff[len_sum], size - len_sum, "%d\t%s\n",
				t->pid, t->comm);
		len_sum += len;
		if (len == 0)
			break;
	}

	retval = simple_read_from_buffer(u_buff, count, offp, k_buff, len_sum);

	kfree(k_buff);

	return retval;
}

static const struct file_operations pt_fops = {
	.read = pt_read,
};

static __init int pt_init(void)
{
	pt_entry = proc_create(pt_filename, 0444, NULL,
					&pt_fops);

	if (pt_entry == NULL) {
		pr_debug("Unable to create proc '%s'\n", pt_filename);
		return -ENOMEM;
	}

	return 0;
}

static __exit void pt_exit(void)
{
	proc_remove(pt_entry);
}

module_init(pt_init);
module_exit(pt_exit);

MODULE_LICENSE("GPL");
