

CC ?= gcc
CXX ?= g++

SRCS = $(wildcard *.cpp *.c)
#OBJS =$(SRCS: *.o=*.c *.o=*.cpp)
OBJS = $(subst .c,.o,$(SRCS))
LDFLAGS= -pthread
CFLAGS = -g -pthread 
CXXFLAGS = -g

INC_DIR	= -I. -I./include -I./include/openssl
LIB_DIR	= -L. -L./lib

LIBS =  -lcrypto -lssl -ldl


all: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(INC_DIR) $(LIB_DIR)  $(LIBS) -o main 

clean:
	rm -f main
	rm -f *.o
#Rules
%.o:%.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@

