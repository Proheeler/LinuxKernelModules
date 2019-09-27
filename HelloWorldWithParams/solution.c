#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <checker.h>
MODULE_LICENSE("GPL");


int param_var=0;

module_param(param_var,int,S_IRUSR | S_IWUSR);

void display(void){
	printk(KERN_ALERT "param_var = %d",param_var);
}
static int hello_init(void){
    printk(KERN_ALERT "Hello, world\n");
    display();
    return 0;
}

static void hello_exit(void){
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
