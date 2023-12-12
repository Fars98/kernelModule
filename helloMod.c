#include<linux/module.h>
#include<linux/kernel.h>

int init_module()
{
	printk(KERN_INFO "Kernel module loaded\n");
	return 0;
}


void cleanup_module()
{
	printk(KERN_INFO "Kernel module unloaded\n");
}
