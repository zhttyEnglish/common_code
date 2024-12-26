#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int var1 = 0, var2 = 0, arr[3], num;
static char buf[10], * str = NULL;

module_param(var1, int, 0644);
MODULE_PARAM_DESC(var1, "get value from user\n");

module_param_named(var2_out, var2, int , 0644);
MODULE_PARAM_DESC(var2, "test var2 named var2_out\n");

module_param(str, charp, 0444);
MODULE_PARAM_DESC(str, "test module str\n");

module_param_string(buf, buffer, sizeof(buf), 0644);
MODULE_PARAM_DESC(buf, "test module buf\n");

module_param_array(arr, int, &num, 0444);
MODULE_PARAM_DESC(arr, "test module arr\n");

static int __init hello_init()
{
    int i = 0;
    printk("--------------------------------------\n");
    printk("var1 : %d \n", var1);
    printk("var2 : %d \n", var2);
    printk("str : %s \n", str);
    printk("buf: %s \n", buf);

    for(i = 0; i < num; i++){
        printk("arr[%d] : %d \n", i, arr[i]);
    }
    printk("-------------------------------------\n");
    return 0;
}

static void __exit hello_exit()
{
    printk("hello exit\n");
    return ;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hyt");
