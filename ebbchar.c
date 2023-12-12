#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "ebbchar"
#define CLASS_NAME "ebb"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_VERSION("0.1");

static int majorNumber;
static struct class* ebbcharClass = NULL;
static struct device* ebbcharDevice = NULL;
static struct cdev my_cdev;
static int numberOpens=0;
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored

static int dev_open(struct inode* inodep, struct file* filep) {
   numberOpens++;
   printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t* offset) {
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
   return 0;

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }

}

static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t* offset) {

        sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
        size_of_message = strlen(message);                 // store the length of the stored message
        printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
        return len;
}

static int dev_release(struct inode* inodep, struct file* filep) {
        printk(KERN_INFO "EBBChar: Device successfully closed\n");
        return 0;
}

static struct file_operations fops = {
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init ebbchar_init(void) {
   dev_t dev;

   if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
      printk(KERN_ALERT "EBBChar failed to register a major number\n");
      return -1;
   }

   majorNumber = MAJOR(dev);

   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)) {
      unregister_chrdev_region(dev, 1);
      printk(KERN_ALERT "Failed to register device class\n");
      return -1;
   }

   if (device_create(ebbcharClass, NULL, dev, NULL, DEVICE_NAME) == NULL) {
      class_destroy(ebbcharClass);
      unregister_chrdev_region(dev, 1);
      printk(KERN_ALERT "Failed to create the device\n");
      return -1;
   }

   cdev_init(&my_cdev, &fops);
   my_cdev.owner = THIS_MODULE;

   if (cdev_add(&my_cdev, dev, 1) < 0) {
      printk(KERN_ALERT "EBBChar failed to add the character device\n");
      device_destroy(ebbcharClass, dev);
      class_destroy(ebbcharClass);
      unregister_chrdev_region(dev, 1);
      return -1;
   }

   printk(KERN_INFO "EBBChar: device class registered correctly\n");
   return 0;
}

static void __exit ebbchar_exit(void) {
   cdev_del(&my_cdev);
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));

