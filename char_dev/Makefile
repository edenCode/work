ifeq ($(KERNELRELEASE),)
KERN_DIR ?= /lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)
#echo $(KERN_DIR)

modules: clean
	$(MAKE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERN_DIR) M=$(PWD) clean
else
	obj-m := freg.o
endif