include ../config.mak

CC  ?= gcc
CXX ?= g++
AR	?= ar

PREFIX ?= .
LIB    	= libcrylib.a
BIN		= $(PREFIX)/server

CFLAGS  = -g -pthread
XFLAGS  =
LDFLAGS = cr

INC_DIR = -I. -I../include/
LIB_DIR = -L. -L../lib/

LIBS	= -lssl -lcrypto -ldl -lstdc++

SRCS 	= $(wildcard  *.c)
#OBJS	= $(subst .cpp,.o,$(SRCS))
OBJS   += $(subst .c,.o,$(SRCS)) 

all:$(OBJS)
	$(AR) $(LDFLAGS) $(LIB) $(OBJS) 

clean:
	rm -f *.o
	rm -f $(LIB)

include rules.mk


