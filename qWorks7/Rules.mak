# Base directories
HOMEDIR := $(TOPDIR)
BASEDIR := $(HOMEDIR)/build

# Hardware platform, ti646x, ti36x, ti812x, ti385 etc.
HARDWARE := ti812x

# Debug switch, YES or NO.
DBG_ON := YES

# Install directory when type 'make install'
LIBDIR := $(HOMEDIR)/lib

ARCH := arm
ifeq ($(HARDWARE),ti646x)
CROSS_COMPILE := /home/youyq/mvt5.1/bin/arm_v5t_le-
endif
ifeq ($(HARDWARE),ti812x)
#CROSS_COMPILE :=
CROSS_COMPILE :=arm-none-linux-gnueabi-
endif

# Power options, Values are "NO" and "YES"
POWER_OPT_ISS_ON := NO
POWER_OPT_DSP_ON := YES
POWER_OPT_VM3_ON := YES
POWER_OPT_DM3_ON := YES

export ARCH CROSS_COMPILE DBG_ON
export BASEDIR HOMEDIR HARDWARE LIBDIR 
export POWER_OPT_ISS_ON POWER_OPT_DSP_ON POWER_OPT_VM3_ON POWER_OPT_DM3_ON 

# End of file

