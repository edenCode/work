#
# Kernel Module Make File
#

# Base directory
BASE_DIR :=  ~/Ti8127

# Architecture and cross compile
ARCH := arm
CROSS_COMPILE := $(BASE_DIR)/arm-2009q1/bin/arm-none-linux-gnueabi-

# Kernel source tree
KERNDIR := $(BASE_DIR)/kernel

ifneq ($(KERNELRELEASE),)
# call from kernel build system

# Extra C compile flags
EXTRA_CFLAGS += -I$(M)/../inc

# Object file(s) to build kernel module, more dependency objects may included.
NCSC_OBJS := ../src/ncsc.o

ncsc-objs := $(NCSC_OBJS)

obj-m := ncsc.o

else

PWD     := $(shell pwd)
SUBDIRS := build src

# Normal Makefile
#include $(HOMEDIR)/Rules.mak
all:
	@$(MAKE) -s -C $(KERNDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

endif

# Cleanup your generated files
clean:
	for i in $(SUBDIRS) ; do cd ../$$i; rm -rf *.ko *.o *~ core .depend .*.cmd *.mod.c .tmp_versions; cd -; done

depend:

.PHONY : all clean

# End of file

