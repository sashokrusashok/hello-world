/*Базовым понятием модели представления устройств являются объекты struct kobject (определенные в файле <linux/kobject.h>). Этот тип определяет общую функциональность, такую как 
счетчик ссылок, имя, указатель на родительский объект, что позволяет создавать объектную иерархию. Зачастую объекты struct kobject встраиваются в другие структуры данных, 
после чего те приобретают свойства, присущие struct kobject. Вот как это выражается в определении уже встречавшейся ранее структуры представления символьного устройства:
struct cdev { 
   struct kobject           kobj; 
   struct module           *owner; 
   struct file_operations  *ops; 
   ...
};
Во внешнем представлении в каталоге /sys каждому объекту struct kobject соответствует каталог, что видно и из самого определения структуры:
struct kobject { 
...
   struct kobj_type *ktype;
   struct dentry    *dentry; 
};
Для того, чтобы сделать объект видимым в /sys, необходимо вызвать:
int kobject_add( struct kobject *kobj );
Хотя это и не делается в примерах, представленных ниже, так как используемые для регистрации имён в /sys вызовы API (class_create()) высокого уровня берут эту задачу на себя.
Файловая система sysfs - это дерево каталогов без файлов. А как создать файлы в этих каталогах, в содержимом которых отображаются данные ядра? Каждый объект struct kobject (каталог) 
содержит (через свой компонент struct kobj_type) массив структур struct attribute.
struct kobj_type {
...
   struct sysfs_ops  *sysfs_ops;
   struct attribute **default_attrs;
}
Каждая такая структура struct attribute (определенная в <linux/sysfs.h>) и является определением одного файлового имени, содержащегося в рассматриваемом каталоге:
struct attribute {
...
   char   *name         имя атрибута-файла ;
   mode_t  mode struct  права доступа к файлу ;
}
Показанная там же структура таблицы операций (struct sysfs_ops) содержит два поля — определения функций show() и store(), соответственно, чтения и записи символьного поля данных 
ядра, отображаемых этим файлом (сами функции и их прототипы показаны в примере ниже).
Различия систем /proc и /sys складываются, главным образом, на основе негласных соглашений и устоявшихся традиций:
информация терминальных имён /proc — комплексная, обычно содержит большие объёмы текстовой информации, иногда это таблицы, и даже с заголовками, проясняющими смысл столбцов таблицы;
информацию терминальных имён /sys (атрибутов) рекомендуется оформлять в виде:
1.простых;
2.символьных изображений;
3.представляющих значения, соответствующие скалярным типам данных языка C (int, long, char[]);*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/sysfs.h> 
#include <linux/kobject.h> 
MODULE_LICENSE("GPL");
#define DEVICE_NAME "tyu"
#define LEN_MSG 160
static char buf_msg[ LEN_MSG + 1 ] = "Hello from module!\n", *msg_ptr = "sdfg";
static int fw_major;
 int accepted_num = 0; 
static struct device* fw_device;
static struct class *fw_class;
static struct kobject *example_kobject;
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset);
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset);
//static ssize_t x_show( struct class *class, struct class_attribute *attr, char *buf );
//static ssize_t x_store( struct class *class, struct class_attribute *attr,const char *buf, size_t count );
static struct file_operations file_ops = {
 .owner = THIS_MODULE,
 .read = device_read,
 .write = device_write,
};
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) 
{
   return simple_read_from_buffer(buffer, len, offset, msg_ptr, strlen(msg_ptr));
}                                   
/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) 
{
   return simple_read_from_buffer(msg_ptr, strlen(msg_ptr), offset, buffer, len);
}
// Метод show(чтения) из sysfs API.Вызываетcя при вызове метода метод show() конкретного файла в системе sysfs 
static ssize_t foo_show( struct class *class, struct class_attribute *attr, char *buf ) 
{
   strcpy( buf, buf_msg );
   printk( "read %d\n", strlen( buf ) );
   return strlen( buf );
}
 
// Метод store() из sysfs API. Вызываетcя при вызове метода store() конкретного файла в системе sysfs 
static ssize_t foo_store( struct class *class, struct class_attribute *attr,const char *buf, size_t count ) 
{
   printk( "write %d\n" , count );
   strncpy( buf_msg, buf, count );
   buf_msg[ count ] = '\0';
   return count;
}
static ssize_t sys_read_accepted_msg(struct device *dev, struct device_attribute *attr, char *buffer) 
{
    return sprintf(buffer, "%u", accepted_num);
} 
static struct kobj_attribute foo_attribute =__ATTR(foo, 0660, foo_show,foo_store);
//static DEVICE_ATTR(acceptedMessages, S_IWOTH | S_IROTH, sys_read_accepted_msg, NULL);
struct class_attribute class_attr_foo = __ATTR(foo, 0660, foo_show,foo_store);
//static CLASS_ATTR(xxx, 0666, &x_show, &x_store); 
int __init x_init( void ) 
{
   int res;
  // int accepted_num = 0;
   int error = 0;
   //const struct class_attribute *class_attr_xxx;
   fw_major = register_chrdev(0, DEVICE_NAME, &file_ops);
   if (fw_major < 0) 
      printk("failed to register device: error %d\n", fw_major); 
   fw_class = class_create(THIS_MODULE, DEVICE_NAME);    
   if (fw_class == NULL) 
         printk("failed to register device class '%s'\n", "x-class");
   /*fw_device = device_create(fw_class, NULL, MKDEV(fw_major, 0), NULL, "etx_device");
   if (fw_device == NULL) 
      printk("failed to create devic");*/
    example_kobject = kobject_create_and_add("kobject_example",kernel_kobj);
    if(!example_kobject)
        return -ENOMEM;
    error = sysfs_create_file(example_kobject, &foo_attribute.attr);
    if (error) 
        pr_debug("failed to create the foo file in /sys/kernel/kobject_example \n");
  // создаётся класс struct class (и соответствующая ему структура struct kobject), отображаемый вызовом class_create() в создаваемом каталоге с именем "x-class". 
  //Далее создаётся переменная class_attr_xxx, содержащая атрибутную запись типа struct class_attribute.
    res = class_create_file( fw_class, &class_attr_foo);
    printk( "'xxx' module initialized\n" );
    return 0;
}
 
void x_cleanup( void ) 
{
    class_remove_file( fw_class, &class_attr_foo );
  unregister_chrdev(fw_major , DEVICE_NAME);
  class_destroy( fw_class );
   
   return;
}
 
module_init( x_init );
module_exit( x_cleanup );