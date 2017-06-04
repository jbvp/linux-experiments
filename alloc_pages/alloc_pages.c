#include <linux/module.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/mm.h>

#define AP_ORDER 1
static struct page *page;

static int __init ap_init(void)
{
	unsigned int i;

	page = alloc_pages(GFP_KERNEL, AP_ORDER);
	if (!page)
		return -ENOMEM;

	for (i = 0; i < 2; i++) {
		pr_debug("page_address = %p\n", page_address(page));
		page += sizeof(struct page);
	}

	return 0;
}

static void __exit ap_exit(void)
{
	__free_pages(page, AP_ORDER);
}

module_init(ap_init);
module_exit(ap_exit);
