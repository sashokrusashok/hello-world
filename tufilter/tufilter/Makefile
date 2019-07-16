obj-m += tufilter.o

all:	
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		sudo dmesg -C
		sudo insmod tufilter.ko
		sudo mknod /dev/ioctl_dev c 247 0

all1:
		make -C /home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/linux-3.10 ARCH=mips CROSS_COMPILE=/home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/toolchain/msdk-4.4.7-mips-EL-3.10-u0.9.33-m32t-140827/bin/mips-linux- M=$(PWD) modules
	
all1_clean:
		make -C /home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/linux-3.10 ARCH=mips CROSS_COMPILE=/home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/toolchain/msdk-4.4.7-mips-EL-3.10-u0.9.33-m32t-140827/bin/mips-linux- M=$(PWD) clean

clean:
		sudo rm /dev/ioctl_dev
		sudo rmmod tufilter
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean