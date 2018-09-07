#include "uart.h"
#include "qWorks.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file.h"
#include "string.h"

#include <sys/select.h>

       /* According to earlier standards */
       #include <sys/time.h>
       #include <sys/types.h>

#define DEV_NAME "/dev/ttyUSB0"

typedef enum{
	MAGICHead =0xFFFFFFFF,
}Magic_head;
typedef struct Header{
	int  magic; // start code,eg : 0xFFFFFFFF
	int size; //	data size 
	int psize; //  packet size
	int count; //  size / psize , if(lsize == 0) size = count * psize;
	int lsize; //  size % psize， if(lszie != 0) size = count * psize + lsize;
}Header;


 UART_BAUDRATE GetBaudrate_( int baudrate )
{
	UART_BAUDRATE rate = UART_BAUDRATE_B115200;

	switch (baudrate) {
		case 0  : rate = UART_BAUDRATE_B1200; break;
		case 1  : rate = UART_BAUDRATE_B1800; break;
		case 2  : rate = UART_BAUDRATE_B2400; break;
		case 3  : rate = UART_BAUDRATE_B4800; break;
		case 4  : rate = UART_BAUDRATE_B9600; break;
		case 5 : rate = UART_BAUDRATE_B19200; break;
		case 6 : rate = UART_BAUDRATE_B38400; break;
		case 7 : rate = UART_BAUDRATE_B57600; break;
		case 8: rate = UART_BAUDRATE_B115200; break;
		default: break;
	}
	return rate;
}

int readTimeOut(int msec, int fd)
{
	int ret = 0;
	struct timeval tv;
	fd_set readfd;
	
	tv.tv_sec = msec/1000;
	tv.tv_usec = msec % 1000 * 1000;
	
	FD_ZERO(&readfd);
	FD_SET(fd, &readfd);
	
	ret = select(fd + 1, &readfd, NULL, NULL, &tv);
	return  ret;
}

#define MAGIC 0xFFFFFFFF
int main(int argc,const char * argv[])
{
	int i = 0;
	int retval = -1;
	char magic[4]={0xff,0xff,0xff,0xff};
	Header head,head1;
	int p_len = 1024;
	int size = 1024;
	int fd = -1;
	FILE * fp = NULL;
	char buffer[1024];
	int rsize = 0;
	UART_ATTRS attrs;
	
	if(argc < 2)
	{
		printf(" Usage : %s brate \n",argv[0]);
		return -1;
	}
			/* Use default attributes */
		attrs.rate   = GetBaudrate_(atoi(argv[1]));
		attrs.size   = UART_DATASIZE_DEFAULT;
		attrs.stop   = UART_STOPBIT_DEFAULT;
		attrs.parity = UART_PARITY_DEFAULT;
	//printf("");
	fd = Uart_Open(4,&attrs);
	if(fd < 0) { printf("error open uart \n"); return -1;}
	
	fp = fopen("/opt/sys/1.jpg","r");
	if(!fp){ printf("fopen file failed \n"); return -2;}
	//head.magic = magic;
	
	fseek(fp,0,SEEK_SET);
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	rewind(fp);
	printf("file size : %d \n",size);
	
	//memcpy(head.magic,magic,4);
	//strncpy(head.magic,magic,4);
	//printf(" atoi %d  \n",atoi((char *)&magic[0]));
	head.magic = 0xf8f7;
	printf("head.magic %d \n",head.magic);
	head.psize = 1024;
	head.size = size;
	head.count = size / head.psize;
	head.lsize = size % head.psize;
	
	printf("Head_size %d ,file size %d,packet size %d,packet counts %d,last size %d \n",sizeof(head),head.size,head.psize,head.count,head.lsize);
	
	retval = write(fd,(char *)&head,sizeof(head)); // send head
	printf("Uart send ： %d bytes \n",retval);
	usleep(1000);
	while(1)
	{
		if(readTimeOut(200,fd) > 0){
			retval = read(fd,(char *)&head1,sizeof(head1)); // recv back
			printf("Uart recv retval : %d \n",retval);
			break;
		}
		else
		{
			printf("recv time out \n");
			sleep(3);
		}
	}
	
	// send image data;	
	for(i = 0; i < head.count; i ++)
	{
		rsize += fread(buffer,1,p_len,fp);
		retval = write(fd,buffer,p_len); 
		printf("Uart send ： %d bytes \n",retval);
	}
	
	if(head.lsize > 0)
	{
		rsize +=fread(buffer,1,head.lsize,fp);
		retval = write(fd,buffer,head.lsize); 
		printf("Uart send ： %d bytes \n",retval);
	}
	
	return 0;
}