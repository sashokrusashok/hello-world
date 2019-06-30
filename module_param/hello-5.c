#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Peter Jay Salzman");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";

/* 
 * module_param(foo, int, 0000) - значения параметров кот могут передаться с командной строки при вызове insmmod, пример ниже, если не вызываются то используется значение по умолчанию
 * Первый параметр -- имя переменной,
 * Второй -- тип,
 * Последний -- биты прав доступа
 * для того, чтобы выставить в sysfs (если ненулевое значение) на более поздней стадии.
 */

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer"); // просто описание модуля
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");

static int __init hello_5_init(void)
{
        printk(KERN_ALERT "Hello, world 5\n=============\n");
        printk(KERN_ALERT "myshort is a short integer: %hd\n", myshort);
        printk(KERN_ALERT "myint is an integer: %d\n", myint);
        printk(KERN_ALERT "mylong is a long integer: %ld\n", mylong);
        printk(KERN_ALERT "mystring is a string: %s\n", mystring);
        return 0;
}

static void __exit hello_5_exit(void)
{
        printk(KERN_ALERT "Goodbye, world 5\n");
}

module_init(hello_5_init);
module_exit(hello_5_exit);
/*
satan# insmod hello-5.ko mystring="bebop" myshort=255
myshort is a short integer: 255
myint is an integer: 420
mylong is a long integer: 9999
mystring is a string: bebop
*/