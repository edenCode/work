ifndef $(HEADER_MK)
HEADER_MK = 1

# Include the make variables (CC, etc...)
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)gcc

# AR flags
AROPTS := -cr

# CC flags
CCOPTS := -Wall -O3

ifeq ($(HARDWARE),ti812x)
CCOPTS += -DTI812X
endif

ifeq ($(DBG_ON),YES)
CCOPTS += -DDBG_ON
else
CCOPTS += -O3
endif

ifeq ($(POWER_OPT_ISS_ON),YES)
CCOPTS += -DPOWER_OPT_ISS_ON
endif
ifeq ($(POWER_OPT_DSP_ON),YES)
CCOPTS += -DPOWER_OPT_DSP_ON
endif
ifeq ($(POWER_OPT_VM3_ON),YES)
CCOPTS += -DPOWER_OPT_VM3_ON
endif
ifeq ($(POWER_OPT_DM3_ON),YES)
CCOPTS += -DPOWER_OPT_DM3_ON
endif

# LD flags
LDOPTS := -lpthread -D_REENTRANT
LDOPTS += -lrt -lm

DEFINES = #-D_INTERNEL_

include $(BASEDIR)/Includes.mk

INCLUDE += -I$(HOMEDIR)/inc
INCLUDE += -I$(DRV_DIR)
INCLUDE += -I$(NET_DIR)
INCLUDE += -I$(OSA_DIR)
INCLUDE += -I$(UTIL_DIR)

endif

