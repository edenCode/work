

CC ?= gcc
CXX ?= g++

SRCS = $(wildcard *.cpp *.c)
OBJS =$(subst .cpp,.o,$(SRCS))

LDFLAGS=
CFLAGS = -g 
CXXFLAGS = -g

INC_DIR	= -I. -I./include -I./include/BasicUsageEnvironment  -I./include/groupsock  -I./include/liveMedia  -I./include/mp4v2  -I./include/UsageEnvironment 
LIB_DIR	= -L. -L./lib

#
#libBasicUsageEnvironment.a  libgroupsock.a  libliveMedia.a  libmp4v2.a  libmp4v2.la  libmp4v2.so.2.0.0  libUsageEnvironment.a
#
LIBS = -lpthread -lBasicUsageEnvironment -lUsageEnvironment -lgroupsock -lliveMedia -lmp4v2 


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

