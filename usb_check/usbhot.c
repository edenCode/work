#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <sys/un.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/types.h> 
#include <linux/netlink.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __ARM__
#include "ncsc.h"
#endif

#include "usb_boot.h"

#define UEVENT_BUFFER_SIZE 2048 

static int init_hotplug_sock() 
{ 
    const int buffersize = 1024; 
    int ret; 

    struct sockaddr_nl snl; 
    bzero(&snl, sizeof(struct sockaddr_nl)); 
    snl.nl_family = AF_NETLINK; 
    snl.nl_pid = getpid(); 
    snl.nl_groups = 1; 

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT); 
    if (s == -1)  
    { 
        perror("socket"); 
        return -1; 
    } 
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize)); 

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)); 
    if (ret < 0)  
    { 
        perror("bind"); 
        close(s); 
        return -1; 
    } 

    return s; 
} 
#ifdef __ARM__
void * usb_contorl(void * param)
{
	int ret = -1;
	int fd = -1;
	
	fd = open("/dev/ncsc",O_RDWR);
	if(fd < 0) printf("open /dev/ncsc failed \n");
	
	while(1)
	{
		ioctl(fd,NCSC_IOCTL_CMD_MAKE(NCSC_CMD_ENABLE),0);
		sleep(3);
		ioctl(fd,NCSC_IOCTL_CMD_MAKE(NCSC_CMD_ENABLE),1);
		sleep(5);
		printf("change usb status \n");
	}
	close(fd);
}
#endif
void * recv_usb_msg(void * param)
{
	int hotplug_sock = init_hotplug_sock();
	
	while(1) 
    { 
        /* Netlink message buffer */ 
        char buf[UEVENT_BUFFER_SIZE * 2] = {0}; 
        recv(hotplug_sock, &buf, sizeof(buf), 0); 
		printf("%s\n", buf); 

        /* USB 设备的插拔会出现字符信息，通过比较不同的信息确定特定设备的插拔，在这添加比较代码 */
	
    }
	
}
int main(int argc, char* argv[]) 
{ 
	pthread_t tid;
	pthread_t tid1;
#ifdef __ARM__	
	pthread_create(&tid,NULL,usb_contorl,NULL);
#endif
	pthread_create(&tid1,NULL,recv_usb_msg,NULL);
	while(1)
	{
		sleep(1);
	}
    return 0; 
}