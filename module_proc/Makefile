obj-m += lkm_example.o
all:	
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		#make -C /usr/src/linux-$(shell uname -r) SUBDIRS=$PWD modules
clean:
		#sudo rmmod lkm_example
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	    # We put a — in front of the rmmod command to tell make to ignore an error in case the module isn’t loaded.
	  	#-sudo rmmod lkm_example
		# Clear the kernel log without echo
		sudo dmesg -C
		# Insert the module
		sudo insmod lkm_example.ko
		# Display the kernel log
		dmesg