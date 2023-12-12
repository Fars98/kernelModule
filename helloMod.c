#define __KERNEL__         
#define MODULE             

#include <linux/module.h>     
#include <linux/kernel.h>     
#include <linux/init.h>       

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Farseena");
MODULE_DESCRIPTION("A hello world kernel module");

int __init init_hello(void);
void  __exit cleanup_module(void);

int __init init_hello()
{
    printk("Hello world... \n");
    return 0;

}

void  __exit cleanup_module() 
{
  printk("Kernel module unloaded \n");
}

module_init(init_hello);

