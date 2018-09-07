#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <termios.h>
#include <stropts.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include "md5.h"

//#define DEV_NAME "/dev/ttyUSB0"
//#define DEV_NAME "/dev/ttyS0"
//#define DEV_NAME "/dev/ttyS1"
//#define DEV_NAME "/dev/ttyS2"


#define MD5_SIZE 32


int num_;
int time;
int uart_fd_ = -1;


typedef struct Header{
	char magic[4]; // start code,eg : 0xFFFFFFFF
	char name[128];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
	char MD5[MD5_SIZE];
}Header;


static int MD5_code(unsigned char *output, int *out_len, const unsigned char *input, int len)
{
    /* MD5 校验 */
    unsigned char mc[MD5_SIZE];


    /* 生成md5校验码 */
    md5_hash(mc, input, len);
    memcpy(output, mc, (*out_len <= MD5_SIZE ? *out_len : MD5_SIZE));
    *out_len = (*out_len <= MD5_SIZE ? *out_len : MD5_SIZE);
    return MD5_SIZE;
}


static UART_BAUDRATE GetBaudrate_( int baudrate )
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

int send_packet(int fd,char * name,char * buffer,int size) // 分包发一张图片
{
	char magic[4]={0xff,0xff,0xff,0xff};
	Header head,headb;
	int i = 0;
	int rlen = 0;
	int retval = 0;
	int psize;
	char * tmp = buffer;
	int tatol = 0;
	char * p = NULL;
	int olen = 32;
	
	memcpy(head.magic,magic,4);
	//if(size >)
	head.psize = 1024*num_;
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
	}
	
	sprintf(head.name,"%s",name);

	
	printf("Head_size %d ,file size %d,packet size %d,packet counts %d,last size %d \n",sizeof(head),head.size,head.psize,head.counts,head.num);
	
	rlen = size;
	psize = head.psize;
	
	if(!p) 
	{
		p = malloc(sizeof(head) + head.psize);
		printf("packet len %d \n",head.psize + sizeof(head));
	}
	
	retval = write(fd,&head,sizeof(head));
	tcflush(fd, TCIOFLUSH);
	for(i = 1; i <=head.counts;i ++)
	{
		memset(p,0,sizeof(head) + head.psize); // 
		
		
		//printf(" >>>>>>> image  name %s  \n",head.name);
		
		if(rlen >=  psize)
		{
			#if 0
			MD5_code(head.MD5,&olen,buffer,head.psize);
			printf("packnum %d,md5 : %s ,md5 len %d\n",head.num,head.MD5,olen);
			
			//memset();
			memcpy(p,&head,sizeof(head));
			
		
			printf(" >>>>>>>  packet id  %d ,l_len %d ,psize %d,head size %d ,size %d\n",i,rlen,head.psize,sizeof(head),size);
			memcpy(p + sizeof(head),buffer,head.psize);
			
			
			tatol = write(fd,p,head.psize + sizeof(head)); // send body
			tcflush(fd, TCIOFLUSH);
			//printf("Uart send ： %d bytes \n",tatol);
			//if(buffer < )
			buffer = tmp + head.psize * i;
			rlen = size - i* head.psize;
			printf("Uart send : %d bytes ,left rlen %d , \n",tatol,rlen);
			#endif
			
			retval = write(fd,buffer,head.psize);
			buffer += retval;
			tatol +=retval;
			rlen -= retval;
		}
		else
		{	
	
		
			#if 0
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
			tcflush(fd, TCIOFLUSH);
			//printf("Uart send ： %d bytes \n",tatol);
			//buffer += head.psize * i;
			//rlen = size - i* head.psize;
			printf(" >>>>>> packet num %d Uart send ： %d bytes ,left rlen %d , \n",i,tatol,rlen);
			#endif
			
			retval = write(fd,buffer,rlen);
			printf("retval %d,rlen %d \n",retval,rlen);
			tatol += retval;
		}
		
		tcflush(fd, TCIOFLUSH);
		usleep(time*1000);
		printf("tatol bytes %d,rlen %d,size %d \n",tatol,rlen,size);
		#if 0
		if(retval >= 0)
		{
			//printf("packet num : %d,packetsize %d ,tatol %d,rlen %d\n",head.num,head.psize,tatol,rlen);
			head.num ++;
		}
				
	
		//Todo recv
		int j = 0;
		memset(&headb,0,sizeof(headb));
		
		if( readTimeOut(time *100,fd) > 0) 
		{
			// 有数据读，判断是否正确，不正确，发上次的包
			retval = read(fd,&headb,sizeof(headb));
			if(retval < 0) printf("Uart recv failed \n");
			printf(">>>>>> retval %d ");
			if(memcmp(headb.magic,magic,4) != 0)
			{
				printf("----test head:");
				for(j = 0;j < 4; j ++)
				{
					printf("%c",headb.magic[i]);
				}
				printf("\n");
				tatol = write(fd,p,head.psize + sizeof(head)); // send body
				tcflush(fd, TCIOFLUSH);
				printf("Retry data 111111\n");
			}
			else
			{
				printf("----MD5 check:");
				for(j = 0;j < 4; j ++)
				{
					printf("%c",headb.MD5[i]);
				}
				printf("\n");
				if(memcmp(head.MD5,"AAAA",4) == 0)
				{
					tatol = write(fd,p,head.psize + sizeof(head)); // send body
					tcflush(fd, TCIOFLUSH);
				}
			}
		}
		

		
		printf("Read magic %s ,magic %s \n",headb.magic,magic);

		if(memcmp(headb.magic,magic,4) == 0)
		{
				printf("----test md5 :");
				for(j = 0;j < 4; j ++)
				{
					printf("%c",headb.MD5[j]);
				}
				printf("\n");
				if(strncmp(headb.MD5,"AAAA",4) == 0)
				{
						//tor[headb.num] = headb.num;
						printf(">>>>>>>>> ####### Recv packet id %d,packet nums %d ,packet size %d\n",headb.num,headb.counts,headb.psize);
					#if 0	
						if(headb.psize == psize)
						{
							buffer = tmp + (headb.num - 1) * headb.psize;
						}
						else
						{
							if(headb.num == headb.counts)
							{
								buffer = tmp + size - headb.psize;
							}
						}
						
						MD5_code(headb.MD5,&olen,buffer,headb.psize);
						
						memcpy(p,&headb,sizeof(head));
			
		
						printf(" >>>>>>> ###### Resend  packet num  %d ,tmp %x,tmp + size %p ,buffer %p,l_len %d ,psize %d,head size %d \n",headb.num,tmp,tmp+size,buffer,buffer - tmp,headb.psize,sizeof(head));
						memcpy(p + sizeof(head),buffer,headb.psize););
					#endif
						retval = write(fd,p,headb.psize); //  retry send buffer,
						if(retval < 0) printf("Re send  failed 222 \n");
				}
				else if(strncmp(headb.MD5,"BBBB",4) == 0) // last one packet retry,
				{
					printf("&&&&&& ##### Not wait,send image %s \n",headb.name);
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
	
	
	retval = send_packet(uart_fd_,file,buffer,size);
	
	return retval;
}
#if 1
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
	char * buffer = NULL;
	int rsize = 0;
	UART_ATTRS attrs;
	int num;
	char dev[256];
	
	if(argc < 5)
	{
		printf(" Usage : %s brate  id num time\n",argv[0]);
		return -1;
	}
			/* Use default attributes */
	attrs.rate   = GetBaudrate_(atoi(argv[1]));
	attrs.size   = UART_DATASIZE_DEFAULT;
	attrs.stop   = UART_STOPBIT_DEFAULT;
	attrs.parity = UART_PARITY_DEFAULT;
	//printf(" num : %d \n",atoi(argv[2]));
	
	if(atoi(argv[2]) < 6)
	{
		sprintf(dev,"/dev/ttyO%d",atoi(argv[2]));
		//printf("");
	}
	else
	{
		sprintf(dev,"/dev/ttyUSB0");
	}

	fd = Uart_Open (dev,&attrs);
	if(fd < 0) { printf("error open uart \n"); return -1;}
	
	fp = fopen("/opt/sys/1.jpg","r + b");
	if(!fp){ printf("fopen file failed \n"); return -2;}
	//head.magic = magic;
	
	#if 0
	int flags = fcntl(fd, F_GETFL);
	flags &= ~O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
	#endif
	
	fseek(fp,0,SEEK_SET);
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	rewind(fp);
	printf("dev name %s file size : %d \n",dev,size);
	
	if(!buffer) {buffer =(char *)malloc(size);}
	
	num_ = atoi(argv[3]);
	time = atoi(argv[4]);
	//printf("");
	printf(">>>>>>>>>>>>>>>>>>>> num %d ,time %d <<<<<<<<<<< \n",num_,time);
	printf("packet size %d ,time %d \n",num_ * 1024,time * 1000);
	retval = fread(buffer,1,size,fp);
	printf("fread file size %d \n",retval);
	
	retval = send_packet(fd,"11111111.jpg",buffer,size);
	printf("Uart send size : %d \n",retval);
	
	if(buffer)
		free(buffer);
	if(fd)
		close(fd);
#if 0	
	memcpy(head.magic,magic,4);
	head.psize = size / num;
	head.size = size;
	head.counts = size % head.psize == 0 ? num : num + 1;
	head.num = 0;
	
	printf("Head_size %d ,file size %d,packet size %d,packet counts %d,last size %d \n",sizeof(head),head.size,head.psize,head.counts,head.num);
	
	retval = write(fd,(char *)&head,sizeof(head)); // send head
	printf("Uart send ： %d bytes \n",retval);
	
	retval = read(fd,(char *)&head1,sizeof(head1)); // recv back
	printf("Uart recv retval : %d \n",retval);
	
	// send image data;	
	for(i = 0; i < head.count; i ++)
	{
		rsize += fread(bufferfer,1,p_len,fp);
		
	#ifdef __MD5__
		MD5_code(MD5,MD5_SIZE,bufferfer,p_len);
	#endif
	
		retval = write(fd,bufferfer,p_len);
		
	#ifdef __MD5__
		retval += write(fd,MD5,MD5_SIZE); // 发送MD5 值
	#endif
	
		printf("Uart send ： %d bytes \n",retval);
		retval = 0;
		usleep(1);
	}
	
	if(head.lsize > 0)
	{
		rsize +=fread(bufferfer,1,head.lsize,fp);

		retval = write(fd,bufferfer,head.lsize);
	#ifdef __MD5__
		MD5_code(MD5,MD5_SIZE,bufferfer,head.lsize);
		//retval = write(fd,bufferfer,head.lsize);
		retval += write(fd,MD5,MD5_SIZE); // 发送MD5 值	
	#endif
		printf("Uart send ： %d bytes \n",retval);
	}
#endif	

	return 0;
}
#endif