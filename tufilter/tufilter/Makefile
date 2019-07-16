obj-m += tufilter.o

#/home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/linux-3.10  -   ядро версии 3.10.90 скомпилированное под mips архитектуру
#CROSS_COMPILE - префикс кросскомпилятора под mips

all:
		make -C /home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/linux-3.10 ARCH=mips CROSS_COMPILE=/home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/toolchain/msdk-4.4.7-mips-EL-3.10-u0.9.33-m32t-140827/bin/mips-linux- M=$(PWD) modules
	
clean:
		make -C /home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/linux-3.10 ARCH=mips CROSS_COMPILE=/home/we/projects/rt/rg35_rtl8197_3-4-11c/rtl819x/toolchain/msdk-4.4.7-mips-EL-3.10-u0.9.33-m32t-140827/bin/mips-linux- M=$(PWD) clean