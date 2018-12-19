
CC  = gcc
CPP = g++
LD  = ld

SRCS = $(wildcard *.c)

#OBJS = $(subst %.c,%.o,$(SRCS))
OBJS = $(SRCS:%.c=%.o)

CFLAGS += -g -Wall
LDFLAGS =
INC_DIR	= -I. -I./libyuv/
LIB_DIR	= -L.

LIBS	= -lyuv -lm -lrt

all: $(OBJS)
	$(CC) $(OBJS) -o test

#Rules
#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。
%.o: %.c
	$(CC)  $(CFLAGS) $(INC_DIR) $(LIB_DIR) -c -o $@ $< 
