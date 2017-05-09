/*
 * proc_seq_simple uses the seq_file interface to return the content of a linked
 * list when reading a proc file.
 *
 * In this example, the linked list volontarily doesn't use the kernel linked
 * list implementation.
 *
 * The iterator is a node pointer. So start() and next() must return a pointer
 * to the iterator: a pointer to a node pointer.
 *
 * stop() is called when the user stops reading, even if it's before EOF.
 * If the user doesn't read until EOF, the iterator must be freed in stop()
 * If the user reads until EOF, next() must return a NULL pointer, which is then
 * passed to stop(). So in this case, the iterator must be freed in next(),
 * before returning NULL.
 *
 * References:
 * linux/fs/seq_file.c
 * https://lwn.net/Articles/22355/ (don't forget to read the comments)
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#define SSP_LIST_SIZE 10

static struct proc_dir_entry *ssp_entry;
static char *ssp_filename = "seq_simple";
struct ssp_node {
	loff_t index;
	u64 jiffies;
	struct ssp_node *next;
};
static struct ssp_node *ssp_root_node;
static loff_t ssp_list_size;

void * ssp_start(struct seq_file *m, loff_t *pos)
{
	struct ssp_node **p;
	loff_t i;

	// start() must check if the offset is greater than the content size:
	// 1) for obivious reason at the first call
	// 2) because start() is called again after next() returns NULL
	pr_debug("ssp_start %llu list_size = %llu \n", *pos, ssp_list_size);
	if (*pos >= ssp_list_size)
		return NULL;

	p = kmalloc(sizeof(struct ssp_node *), GFP_KERNEL);
	if (p <= 0)
		return NULL;

	*p = ssp_root_node;
	for (i = 0; i < *pos; i++)
		*p = (*p)->next;

	return p;
}

void ssp_stop(struct seq_file *m, void *v)
{
	pr_debug("ssp_stop %p\n", v);
	// If the user didn't read until EOF, next() didn't freed the iterator
	// and so we have to do it here.
	if (v) { // check only necessary because of the following pr_debug
		pr_debug("ssp_stop kfree(%p)\n", v);
		kfree(v);
	}
}

void * ssp_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct ssp_node **p = (struct ssp_node **)v;
	pr_debug("ssp_next %p\n", p);
	(*pos)++;
	if ((*p)->next) {
		*p = (*p)->next;
		return p;
	}
	// Free the iterator here because stop() will receive a NULL pointer
	// and so will be unable to do it.
	pr_debug("ssp_next kfree(%p)\n", p);
	kfree(p);
	return NULL;
}

int ssp_show(struct seq_file *m, void *v)
{
	struct ssp_node *node = *((struct ssp_node **)v);
	seq_printf(m, "v=%p node=%p index=%llu jiffies=%llu\n",
			v, node, node->index, node->jiffies);
	return 0;
}

static const struct seq_operations ssp_sops = {
	.start = ssp_start,
	.stop = ssp_stop,
	.next = ssp_next,
	.show = ssp_show,
};

static int ssp_open(struct inode *i, struct file *filp)
{
	return seq_open(filp, &ssp_sops);
}

static const struct file_operations ssp_fops = {
	.open = ssp_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static __init int ssp_init(void)
{
	loff_t i;
	struct ssp_node *node;
	// starter_node is a "fake" node located before the first real node,
	// only to be able to assign prev_node->next = node, even at the first
	// loop iteration (avoids an ugly if (i > 0) { prev_node->next = node}).
	struct ssp_node starter_node;
	struct ssp_node *prev_node = &starter_node;

	for (i = 0; i < SSP_LIST_SIZE; i++) {
		node = kmalloc(sizeof(struct ssp_node), GFP_KERNEL);
		if (node == NULL) {
			pr_debug("Unable to kmalloc struct ssp_node %llu", i);
			return -ENOMEM;
		}
		node->index = i;
		node->jiffies = get_jiffies_64()+i; // dummy value for testing
		prev_node->next = node;
		prev_node = node;
		ssp_list_size++;
	}
	node->next = NULL;
	ssp_root_node = starter_node.next;

	ssp_entry = proc_create(ssp_filename, 0444, NULL, &ssp_fops);
	if (ssp_entry == NULL) {
		pr_debug("Unable to create proc '%s'\n", ssp_filename);
		return -ENOMEM;
	}

	return 0;
}

static __exit void ssp_exit(void)
{
	struct ssp_node *prev_node;
	struct ssp_node *node = ssp_root_node;

	proc_remove(ssp_entry);

	while (node) {
		prev_node = node;
		node = node->next;
		kfree(prev_node);
	}
}

module_init(ssp_init);
module_exit(ssp_exit);

MODULE_LICENSE("GPL");
