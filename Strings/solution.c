#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>

#include "checker.h"

MODULE_LICENSE("GPL") ;

typedef struct dynamic_string_simple dynamic_string_simple;
struct dynamic_string_simple
{
    char *buffer;
    size_t pos; //куда писать
    size_t capacity;
}sstr;

void ss_realock(dynamic_string_simple *string_simple, size_t newsize){
    //char *newbuf = (char *)malloc(newsize * sizeof(char));
    char *newbuf = (char *)kmalloc(newsize * sizeof(char), GFP_KERNEL);
    for (int i = 0; i < string_simple->pos; ++i){
        newbuf[i] = string_simple->buffer[i];
    }
    kfree(string_simple->buffer);
    string_simple->buffer = newbuf;
    string_simple->capacity = newsize;
}

dynamic_string_simple* create_sstring(size_t size){
    dynamic_string_simple *string_simple = NULL;
    string_simple = (dynamic_string_simple *)kmalloc(size * sizeof(dynamic_string_simple), GFP_KERNEL);
    string_simple->buffer = (char *)kmalloc(size * sizeof(char), GFP_KERNEL);
    string_simple->pos = 0;
    string_simple->capacity = size;
    return string_simple;
}

void free_sstring(dynamic_string_simple *string_simple){
    kfree(string_simple->buffer);
    kfree(string_simple);
    string_simple = NULL;
}

void append_string(
        dynamic_string_simple *string_simple,
        const char *arr, size_t arr_size){
    if(string_simple->capacity < string_simple->pos + arr_size)
        ss_realock(string_simple, (string_simple->capacity + arr_size) * 2);
    //
    for(int i = 0; i < arr_size; ++i){
        string_simple->buffer[string_simple->pos] = arr[i];
        ++string_simple->pos;
    }
}

void add_char(dynamic_string_simple *string_simple, char c)
{
    if(string_simple->capacity < string_simple->pos + 1)
        ss_realock(string_simple, (string_simple->capacity + 1) * 2);
    string_simple->buffer[string_simple->pos] = c;
    ++string_simple->pos;
}

void fill_sstring(dynamic_string_simple *simple_string,
                  int sum, short *arr, size_t size){
    // <result_from_array_sum> <arr[0]> <arr[1]> ... <arr[n-1]>
    const size_t hb_size = 128;
    char helping_buf[hb_size];
    int copy_size = snprintf(helping_buf, hb_size, "%d ", sum);
    append_string(simple_string, helping_buf, copy_size);
    for(int i = 0; i < size; ++i){
        copy_size = snprintf(helping_buf, hb_size, "%d ", arr[i]);
        append_string(simple_string, helping_buf, copy_size);
    }
    add_char(simple_string, '\0');
}

//***************--------------------***************

void fill_random_array(short *arr, size_t size) {
    for (int i = 0; i<size; ++i) {
        get_random_bytes(arr + i, sizeof(short));
    }
}

int check_array_sum(short *arr, size_t n){
    int sum = 0;
    for(int i = 0; i < n; ++i)
        sum += arr[i];
    return sum;
}

int __init init_module(void) {
    CHECKER_MACRO;
    const int number_of_sums = 10;
    short *arr = NULL;
    for(int i = 0; i < number_of_sums; ++i)
    {
        // умная строка
        dynamic_string_simple *string_simple = create_sstring(128);
        // Заполнение массива чисел
        char size_of_array = 0;
        get_random_bytes ( &size_of_array, sizeof (size_of_array) );
        size_of_array = 11;
        arr = kmalloc(size_of_array * sizeof(short), GFP_KERNEL);
        fill_random_array(arr, size_of_array);
        //
        int sum = array_sum(arr, size_of_array);
        //Создание буфера вывода
        char *buf = NULL;
        //ssize_t res = generate_output(sum, arr, size_of_array, buf);
        int check_sum = check_array_sum(arr, size_of_array);
        //
        fill_sstring(string_simple, sum, arr, size_of_array);
        if(check_sum == sum)
        {
            printk(KERN_INFO "kernel_mooc EQUAL\n kernel_mooc");
            printk(KERN_INFO "kernel_mooc %s\n kernel_mooc", string_simple->buffer);
        }
        else
        {
            printk(KERN_INFO "kernel_mooc NOT EQUAL\n kernel_mooc");
            printk(KERN_ERR "kernel_mooc %s\n kernel_mooc", string_simple->buffer);
        }
        free_sstring(string_simple);
        kfree(arr);
    }
    return 0 ;
}

void exit_module(void) {
    CHECKER_MACRO;
}
module_exit(exit_module);
