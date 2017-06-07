#include <linux/module.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/kernel.h>

#define AP_ORDER 1
static struct page *page;

static int __init ap_init(void)
{
	unsigned int i;
	struct page *p;

	pr_debug("Alloc %d contiguous pages with alloc_pages\n", 1 << AP_ORDER);
	page = alloc_pages(GFP_KERNEL, AP_ORDER);
	if (!page)
		return -ENOMEM;

	for (i = 0, p = page; i < (1 << AP_ORDER); i++, p++) {
		pr_debug("page_address (virtual address) = %p\n",
				page_address(page));
	}

	return 0;
}

static void __exit ap_exit(void)
{
	__free_pages(page, AP_ORDER);
}

module_init(ap_init);
module_exit(ap_exit);
