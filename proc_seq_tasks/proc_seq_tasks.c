/*
 * proc_seq_tasks uses the seq_file interface to display all the tasks.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define PROC_SEQ_TASKS_FILENAME "seq_tasks"
static struct proc_dir_entry *pst_entry;

static void *pst_start(struct seq_file *m, loff_t *pos)
{
	struct list_head **v;

	if (*pos > 0)
		return 0;

	v = kmalloc(sizeof(struct list_head *), GFP_KERNEL);
	if (v <= 0)
		return 0;

	*(struct list_head **)v = &init_task.tasks;
	return v;
}

static void pst_stop(struct seq_file *m, void *v)
{
	kfree(v);
}

static void *pst_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct list_head *lh = *(struct list_head **)v;

	if (lh->next != &init_task.tasks) {
		*(struct list_head **)v = lh->next;
		++*pos;
		return v;
	}
	kfree(v);
	return NULL;
}

static int pst_show(struct seq_file *m, void *v)
{
	struct list_head *lh = *(struct list_head **)v;
	struct task_struct *t = container_of(lh, struct task_struct, tasks);

	seq_printf(m, "%d\t%s\n", t->pid, t->comm);

	return 0;
}

static const struct seq_operations pst_sops = {
	.start = pst_start,
	.next = pst_next,
	.show = pst_show,
	.stop = pst_stop,
};

static int pst_open(struct inode *i, struct file *filp)
{
	return seq_open(filp, &pst_sops);
}

static const struct file_operations pst_fops = {
	.open = pst_open,
	.read = seq_read,
	.release = seq_release,
};

static int __init pst_init(void)
{
	pst_entry = proc_create(PROC_SEQ_TASKS_FILENAME, 0444, NULL, &pst_fops);
	if (!pst_entry)
		return -ENOMEM;
	return 0;
}

static void __exit pst_exit(void)
{
	proc_remove(pst_entry);
}

module_init(pst_init);
module_exit(pst_exit);

MODULE_LICENSE("GPL");
