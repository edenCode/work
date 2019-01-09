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

LIBS	= -lssl -lcrypto -ldl

SRCS 	= $(wildcard  *.cpp)
#OBJS	= $(subst .cpp,.o,$(SRCS))
#OBJS   += $(subst .cpp,.o,$(SRCS)) 
OBJS	+= cryptolib.o



all:$(OBJS)
	$(AR) $(LDFLAGS) $(LIB) $(OBJS) 

test: all 
	$(CC) $(CFLAGS) main.c  -o $@ $(INC_DIR) -L. -lcrylib $(LIB_DIR) $(LIBS)

clean:
	rm -f *.o
	rm -f $(LIB)

include rules.mk


