####################
CC  = gcc
CPP = g++
LD  = ld

SRCS = $(wildcard *.cpp)

#OBJS = $(subst %.c,%.o,$(SRCS))
OBJS = $(SRCS:%.cpp=%.o)

BIN=  $(CURDIR)/test
CFLAGS += -g 
LDFLAGS =
INC_DIR	=  
LIB_DIR	= 
LIBS	= 

all: $(OBJS)
	$(CPP) $(LDFLAGS) $(OBJS) -o $(BIN) $(INC_DIR) $(LIB_DIR) $(LIBS) 
	
clean:
	rm -f $(OBJS)
	rm -f $(BIN)
#Rules
#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。
%.o: %.c
	$(CC)  $(CFLAGS) $(INC_DIR) $(LIB_DIR) $(LIBS) -c -o $@ $< 
	
%.o: %.cpp
	$(CPP)  $(CFLAGS) $(INC_DIR) $(LIB_DIR) $(LIBS) -c -o $@ $< 
