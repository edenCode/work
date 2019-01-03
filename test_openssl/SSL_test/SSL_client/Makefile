include ../config.mak

CC  ?= gcc
CXX ?= g++

PREFIX ?= .
LIB    	= 
BIN	= $(PREFIX)/client

CFLAGS  = -g -pthread
XFLAGS  =
LDFLAGS =

INC_DIR = -I. -I../include/ -I../cryptolib
LIB_DIR = -L. -L../lib/ -L../cryptolib

LIBS	= -lssl -lcrypto -ldl -lstdc++ -lcrylib

SRCS 	= $(wildcard *.cpp)
OBJS	= $(subst .cpp,.o,$(SRCS))

all:$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB_DIR) $(LIBS) -o $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

include rules.mk
