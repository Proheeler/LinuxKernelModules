#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/device.h>
#include "checker.h"
MODULE_LICENSE("GPL");

void *pVoid;
int *pInt;
struct device *pDevice;

static int hello_init(void){
    ssize_t pVoidSize = get_void_size();
    pVoid = kmalloc(pVoidSize, GFP_ATOMIC);
    submit_void_ptr(pVoid);
    //
    ssize_t pIntSize = get_int_array_size();
    pInt = (int*)kmalloc(pIntSize*sizeof(int), GFP_ATOMIC);
    submit_int_array_ptr(pInt);
    //
    pDevice = (struct device*)kmalloc(sizeof(struct device), GFP_ATOMIC);
    submit_struct_ptr(pDevice);

    return 0 ;
}

static void hello_exit(void){
    checker_kfree(pVoid);
    checker_kfree((void*)pInt);
    checker_kfree((void*)pDevice);
}

module_init(hello_init);
module_exit(hello_exit);
