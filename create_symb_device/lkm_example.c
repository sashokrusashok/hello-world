/*Добавление драйвера в систему подразумевает его регистрацию в ядре. Это означает -- получение старшего номера в момент инициализации модуля. После этого создается символьное устройство lkm c мажорным номером,
который мы зарегисрировали с помощью mknod /dev/lkm c 240 0. Когда вызываем команду cat /dev/lkm/, тем самы вызываетс ф-ция read в  файл устройства, которая переадресуется наш модуль с полученным мажорным 
номером. В нем считывается данные из буфера модуля ядра (hello_world) и перезаписываются в буфер пользователя, когда исп команду echo "sdgsdfg" > /dev/lkm, то выз ф-ция write, в которой данные из буфера 
пользователя записываются в буфер модуля ядра, при повторном вызове echo для буфера модуля ядра используется динамический массив, кот выделяется с помощью kmalloc()  и перезаписывается указатель со статического 
массива на динамический,*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");
#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World!\n"
#define MSG_BUFFER_LEN 15
/* Prototypes for device functions */
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int len_ker_buf = 15;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;
static int flag = 0;
/* This structure points to all of the device functions */
static struct file_operations file_ops = {
 .owner = THIS_MODULE,
 .read = device_read,
 .write = device_write,
};
/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) 
{
	return simple_read_from_buffer(buffer, len, offset, msg_ptr, len_ker_buf);
}												
/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) 
{
	/*Если используется функция write то в первый раз переназначается указатель с статич массива на динамич и при первом заходе в write динамич массив не удаляется, а при последующих стираетсяЮ так как
	значение флага изменено, но здесь не стирается самый последний динамически созданный массив, он стирается в функции выгрузки модуля*/
	if(flag)
		kfree(msg_ptr);
	flag = 1;
	len_ker_buf = (int)len;
	msg_ptr = kmalloc(len_ker_buf,GFP_KERNEL);
	simple_write_to_buffer(msg_ptr, len_ker_buf, offset,buffer, len);
	printk(KERN_ALERT "buffer of kernelspace have value: %s", msg_ptr);
}
static int __init lkm_example_init(void) 
{
 	/* Fill buffer with our message */
 	strncpy(msg_buffer, EXAMPLE_MSG, len_ker_buf);
 	/* Set the msg_ptr to the buffer */
 	msg_ptr = msg_buffer;
 	/* Регистрация имени устройства создаёт соответствующую запись в файле /proc/devices, но не создаёт самого устройства в /dev:. 
	Старший номер устройства говорит о том, какой драйвер с каким файлом устройства связан, Младший номер используется самим драйвером для идентификации устройства, если он обслуживает несколько таких устройств.
	Добавление драйвера в систему подразумевает его регистрацию в ядре. Это означает -- получение старшего номера в момент инициализации модуля.
	где unsigned int major -- это запрашиваемый старший номер устройства, const char *name -- название устройства, которое будет отображаться в /proc/devices и struct file_operations, оно не обязательно должно иметь имя самого файла устройства *fops -- указатель на таблицу file_operations драйвера
	Если вы передадите функции register_chrdev(), в качестве старшего номера, число 0, то возвращаемое положительное значение будет представлять собой, динамически выделенный ядром, старший номер устройства.
	*/ 
 	major_num = register_chrdev(0, "lkm_example", &file_ops);
 	if (major_num < 0) 
 	{
 		printk(KERN_ALERT "Could not register device: %d\n", major_num);
 		return major_num;
 	} 
 	else 
 	{
 		printk(KERN_INFO "lkm_example module loaded with device major number %d\n", major_num);
 		return 0;
 	}
}
static void __exit lkm_example_exit(void) 
{
 	/* Remember — we have to clean up after ourselves. Unregister the character device. */
 	/*если флаг равен единицы, то мы использовали write() и надо стереть самый последний массив, если флаг = 0, то мы не использовали echo*/
 	if(flag)
 		kfree(msg_ptr);
 	unregister_chrdev(major_num, DEVICE_NAME);
 	printk(KERN_INFO "Goodbye, World!\n");
}
/* Register module functions */
module_init(lkm_example_init);
module_exit(lkm_example_exit);