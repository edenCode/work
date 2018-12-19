#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ringbuffer.h"

int create_udp()
{
	int sockfd = -1;

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		perror("create socket failed \n");
		return -1;
	}


	return 0;
}


int main()
{
	printf("hello world \n");
	pthread_t tid[2];
	return 0;
}
