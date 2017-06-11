#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>

static unsigned int *g;

static int __init pava_init(void)
{
	unsigned long l_pa, g_pa, l_va, g_va;
	unsigned int l = 42;

	g = kmalloc(sizeof(unsigned int), GFP_KERNEL);
	if (!g)
		return -ENOMEM;

	l_pa = __pa(&l);
	g_pa = __pa(g);
	l_va = __va(l_pa);
	g_va = __va(g_pa);

	pr_debug("&l = %p __pa(&l) = 0x%lx __va(l_pa) = 0x%lx\n", &l, l_pa, l_va);
	pr_debug(" g = %p __pa(g)  = 0x%lx __va(g_pa) = 0x%lx\n", g, g_pa, g_va);

	return 0;
}

static void __exit pava_exit(void)
{
	kfree(g);
}

module_init(pava_init);
module_exit(pava_exit);
