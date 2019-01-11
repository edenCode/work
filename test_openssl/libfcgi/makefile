#include ../config.mak

CC  ?= gcc
CXX ?= g++
AR	?= ar

PREFIX ?= .
LIB    	= libfcgissl.so
BIN		= $(PREFIX)/server

CFLAGS  = -g -pthread -fPIC -DHAVE_OPENSSL_H
XFLAGS  =
LDFLAGS = -shared -fPIC

INC_DIR = -I. -I../include/
LIB_DIR = -L. -L../lib/

LIBS	= -lssl -lcrypto -ldl

SRCS 	= $(wildcard  *.c)
#OBJS	= $(subst .cpp,.o,$(SRCS))
OBJS   += $(subst .c,.o,$(SRCS)) 
#OBJS	+= cryptolib.o



all:$(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o $(LIB) $(INC_DIR) $(LIB_DIR) $(LIBS)

test: all 
	$(CC) $(CFLAGS) main.c  -o $@ $(INC_DIR) -L. -lcrylib $(LIB_DIR) $(LIBS)

clean:
	rm -f *.o
	rm -f $(LIB)

include rules.mk


