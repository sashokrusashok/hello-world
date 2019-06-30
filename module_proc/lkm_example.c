#include <linux/module.h> /* Необходимо для любого модуля */
#include <linux/kernel.h> /* Все-таки мы работаем с ядром! */
#include <linux/proc_fs.h>/* Необходимо для работы с файловой системой /proc */
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/stat.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valerie Henson <val@nmt.edu.>");
MODULE_DESCRIPTION("\"Hello, world!\" minimal module");
MODULE_VERSION("printk");
static int len_ker_buf = 7;
struct proc_dir_entry *our_proc_file;
static ssize_t procfile_read(struct file*, char*, size_t, loff_t*);
struct proc_dir_entry *create_proc_entry( const char *name, mode_t mode,struct proc_dir_entry *parent );
static struct file_operations cmd_file_ops = {
    .owner = THIS_MODULE,
    .read = procfile_read,
};
char *msg_ptr = "Hello\n";
//struct proc_dir_entry proc_root;
static ssize_t procfile_read(struct file *file, char *buffer, size_t len, loff_t *offset)
{
   /* int ret;
    printk(KERN_INFO "inside /proc/test : procfile_read\n");
      
    ret = sprintf(buffer, "Hello, world!\n");
     return ret;*/
    return simple_read_from_buffer(buffer, len, offset, msg_ptr, len_ker_buf);
}

int init_module()
{
  	int rv = 0;
 	printk(KERN_INFO "Trying to create /proc/test:\n");
  	our_proc_file = proc_create("test", S_IFREG | S_IRUGO, NULL,&cmd_file_ops);

    if (our_proc_file == NULL) 
    {
    	rv = -ENOMEM;
    	remove_proc_entry("test", NULL);
    	printk(KERN_INFO "Error: Could not initialize /proc/test\n");
    } 
    else 
    	printk(KERN_INFO "Success!\n");

        proc_set_user(our_proc_file, KUIDT_INIT(0), KGIDT_INIT(0));
    	proc_set_size(our_proc_file, 37);
   // our_proc_file->owner = THIS_MODULE;
    //our_proc_file->mode = S_IFREG | S_IRUGO;
   // our_proc_file->uid = a;
    //our_proc_file->gid = 0;
   // our_proc_file->read_proc = procfile_read;
    //our_proc_file->size = 37;

    return rv;
}

void cleanup_module()
{
  remove_proc_entry("test", NULL);
  printk(KERN_INFO "/proc/test removed\n");
}