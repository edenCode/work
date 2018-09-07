#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stropts.h>
#include <stdio.h>
#include <termios.h>
#include <osFile.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include "md5.h"


typedef struct Header{
	char magic[4]; // start code,eg : 0xFFFFFFFF
//	char name[128];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
	char MD5[MD5_SIZE];
}Header;

static int readTimeOut(int msec, int fFd)
{
	struct timeval tv;
	fd_set readfd;
	
	tv.tv_sec = msec/1000;
	tv.tv_usec = msec % 1000 * 1000;
	
	FD_ZERO(&readfd);
	FD_SET(fFd, &readfd);
	
	return select(fFd + 1, &readfd, NULL, NULL, &tv);
}

int send_packet(int fd,char * buffer,int size) // 分包发一张图片
{
	char magic[4]={0xff,0xff,0xff,0xff};
	Header head,headb;
	int i = 0;
	int rlen = 0;
	int retval = 0;
	int psize;
	int tor[P_NUM];
	char * tmp = buffer;
	int tatol = 0;
	char * p = NULL;
	int t_t  = 0;
	int olen = 32;
	
	memcpy(&t_t,magic,4);
	printf(" t_t %d \n",t_t);
	
	memcpy(head.magic,magic,4);
	//if(size >)
	head.psize = 1024*60;
	head.size = size;

	head.num = 1;
	
	if(head.psize >= size)
	{
		head.psize = size;
		head.counts = 1; 
	}
	else
	{
		if(size % head.psize == 0)
		{
			head.counts = size / head.psize;
		}
		else
		{
			head.counts = size / head.psize + 1;
		}
		printf(" head.counts : %d \n",head.counts);
	}
	
	printf("Head_size %d ,file size %d,packet size %d,packet counts %d,last size %d \n",sizeof(head),head.size,head.psize,head.counts,head.num);
	
	rlen = size;
	psize = head.psize;
	
	if(!p) 
	{
		p = malloc(sizeof(head) + head.psize);
		printf("packet len %d \n",head.psize + sizeof(head));
	}
	
	for(i = 1; i <=head.counts;i ++)
	{
		memset(p,0,sizeof(head) + head.psize); // 
		
		if(rlen >=  psize)
		{
		
			MD5_code(head.MD5,&olen,buffer,head.psize);
			printf("packnum %d,md5 : %s ,md5 len %d\n",head.num,head.MD5,olen);
		
			
			//memset();
			memcpy(p,&head,sizeof(head));
			
		
			printf(" i  %d ,tmp %x,tmp + size %p ,buffer %p,l_len %d ,psize %d,head size %d \n",i,tmp,tmp+size,buffer,buffer - tmp,head.psize,sizeof(head));
			memcpy(p + sizeof(head),buffer,head.psize);
			
			
			tatol = write(fd,p,head.psize + sizeof(head)); // send body
			//printf("Uart send ： %d bytes \n",tatol);
			//if(buffer < )
			buffer = tmp + head.psize * i;
			rlen = size - i* head.psize;
			printf("Uart send ： %d bytes ,left rlen %d , \n",tatol,rlen);
		}
		else
		{	
			
			head.psize = rlen; // left data
			//printf("send left data : %d  packsize %d \n",rlen,head.psize);
	

			MD5_code(head.MD5,&olen,buffer,head.psize);
			printf("packnum %d,md5 : %s , olen %d ,rlen %d\n",head.num,head.MD5,olen,head.psize);


			memcpy(p,&head,sizeof(head));
			if(buffer - tmp >= head.psize)
			{
				memcpy(p+sizeof(head),buffer,head.psize);
			}
			tatol = write(fd,p,head.psize + sizeof(head)); // send body
			//printf("Uart send ： %d bytes \n",tatol);
			//buffer += head.psize * i;
			//rlen = size - i* head.psize;
			printf(" i %d Uart send ： %d bytes ,left rlen %d , \n",i,tatol,rlen);
		
			
		}
		
		if(retval >= 0)
		{
			//printf("packet num : %d,packetsize %d ,tatol %d,rlen %d\n",head.num,head.psize,tatol,rlen);
			head.num ++;
		}
		//Todo recv
	#if 0
		while(1)
		{
			if(readTimeOut(2000,fd) > 0)
			{
				read(fd,&headb,sizeof(headb));
			}
			
			if(strncmp(headb.magic,magic,4) == 0)
			{
				printf("headb.psize %d \n",headb.psize);
				if(headb.num  < head.counts)
				{
					if(headb.psize != psize)
					{
						//tor[headb.num] = headb.num;
						headb.psize = psize;
						
						MD5_code(head.MD5,&olen,buffer,head.psize);
						printf("packnum %d,md5 : %s ,md5 %d \n",head.num,head.MD5,olen);
					
						retval = write(fd,(char *)&headb,sizeof(head)); // send head
						retval = write(fd,tmp + psize * (head.num - 1),psize); //  retry send buffer,
					}
				}
				else if(headb.num == headb.counts) // last one packet retry,
				{
					if(headb.psize != rlen)
					{
						//tor[headb.num] = headb.num;
						headb.psize = rlen; // packet size 
		
						MD5_code(head.MD5,&olen,buffer,head.psize);
						printf("packnum %d,md5 : %s ,olen %d\n",head.num,head.MD5,olen);
						
						retval = write(fd,(char *)&headb,sizeof(head)); // send head
						retval = write(fd,tmp + psize * (head.num - 1),rlen); //  retry send buffer,
					}
				}
			}
		}
		
	 #endif
	}

	return size;
}

int sendImageByUart(char * file,char * buffer,int size)
{
	//int fd = -1;
	int retval = -1;
	UART_ATTRS attrs;
	
	attrs.rate   = UART_BAUDRATE_B115200;
	attrs.size   = UART_DATASIZE_DEFAULT;
	attrs.stop   = UART_STOPBIT_DEFAULT;
	attrs.parity = UART_PARITY_DEFAULT;
	
	if(uart_fd_ < 0)
		uart_fd_ = Uart_Open ("/dev/ttyUSB0",&attrs);
	if(uart_fd_ < 0) { printf("error open uart \n"); return -1;}
	
	retval = send_packet(uart_fd_  ,buffer,size);
	
	return retval;
}