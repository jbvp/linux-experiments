ifneq ($(KERNELRELEASE),)

	obj-m := proc_seq_tasks.o
	CFLAGS_proc_seq_tasks.o := -DDEBUG

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build

default:

	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:

	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

endif
