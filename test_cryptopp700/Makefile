

CC ?= gcc
CXX ?= g++

SRCS = $(wildcard *.cpp *.c)
OBJS =$(subst .cpp,.o,$(SRCS))

LDFLAGS=
CFLAGS = -g 
CXXFLAGS = -g

INC_DIR	= -I. -I./include -I./include/cryptopp
LIB_DIR	= -L. -L./lib

LIBS = -lcryptopp


all: $(OBJS)
	$(CXX) $(OBJS) $(INC_DIR) $(LIB_DIR)  $(LIBS) -o main 

clean:
	rm -f main
	rm -f *.o
#Rules
%.o:%.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@

