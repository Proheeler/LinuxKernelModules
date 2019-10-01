#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>
// Шаблон драйвера символьного устройства


MODULE_LICENSE("GPL") ;
MODULE_DESCRIPTION("This is a very important kernel module");
// sudo mknod /dev/solution_node c 240 0
// sudo chmod a+rw /dev/solution_node
// ls -la /dev/solution_node
// sudo chmod a+rw /dev/solution_node
// sudo echo "hi" > /dev/solution_node

static dev_t first;
static unsigned int open_count = 0;
static size_t bytes_written = 0;
static char was_readed = 0;

static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
// struct cdev
static struct cdev *my_cdev;

static char help_buf[128];

#define DEVICE_NAME "MY_CHR_DEVICE"
#define KBUF_SIZE PAGE_SIZE

// open и release происходять перед КАЖДЫМ чтением-записью
static int mydev_open(struct inode *pinode, struct file *pfile){
    open_count++;
    was_readed = 0;
    // printk( KERN_INFO "mydev_open open_count==%d", open_count);
    return 0;
}

static int mydev_release(struct inode *pinode, struct file *pfile){
    return 0;
}

static ssize_t mydev_read(struct file *pfile, char __user *buf, size_t read_size, loff_t *ppos){
    int nbytes = sprintf(help_buf, "%d %ld\n", open_count, bytes_written);
    int copied = nbytes - copy_to_user(buf, help_buf, nbytes);
    if (!was_readed){
        was_readed = 1;
        return copied;
    }
    return 0;
}

static ssize_t mydev_write(struct file *pfile, const char __user *buf, size_t write_size, loff_t *ppos){
    int nbytes = write_size - copy_from_user(help_buf + *ppos, buf, write_size);
    bytes_written += nbytes;
    return nbytes;
}

struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = mydev_open,
        .read = mydev_read,
        .write = mydev_write,
        .release = mydev_release
};

static int __init hello_init(void) {
    first = MKDEV(my_major, my_minor);
    register_chrdev_region(first, count, DEVICE_NAME);
    my_cdev = cdev_alloc();
    cdev_init(my_cdev, &fops);
    cdev_add(my_cdev, first, count);
    return 0 ;
}

static void __exit hello_exit(void) {
    // printk( KERN_ALERT "Module unloaded\n" ) ;
    if(my_cdev)
        cdev_del(my_cdev);
    unregister_chrdev_region(first, count);
}

module_init(hello_init) ;
module_exit(hello_exit) ;
