include ../config.mak

CC  ?= gcc
CXX ?= g++

PREFIX ?= .
LIB    	= 
BIN	= $(PREFIX)/server

CFLAGS  = -g -pthread
XFLAGS  =
LDFLAGS =

INC_DIR = -I. -I../include/
LIB_DIR = -L. -L../lib/

LIBS	= -lssl -lcrypto -ldl -lstdc++

SRCS 	= $(wildcard *.cpp)
OBJS	= $(subst .cpp,.o,$(SRCS))

all:$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB_DIR) $(LIBS) -o $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

include rules.mk
