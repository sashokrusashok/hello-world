obj-m += tufilter.o

all:	
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		sudo dmesg -C
		sudo insmod tufilter.ko
		sudo mknod /dev/ioctl_dev c 247 0
		
clean:
		sudo rm /dev/ioctl_dev
		sudo rmmod tufilter
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean