#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stropts.h>
#include <termios.h>
#include "md5.h"


#define MD5_SIZE 32



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

static int set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
    struct termios newttys1,oldttys1;

     /*保存原有串口配置*/
     if(tcgetattr(fd,&oldttys1)!=0) 
     {
          perror("Setupserial 1");
          return -1;
     }
     bzero(&newttys1,sizeof(newttys1));
     newttys1.c_cflag|=(CLOCAL|CREAD ); /*CREAD 开启串行数据接收，CLOCAL并打开本地连接模式*/

     newttys1.c_cflag &=~CSIZE;/*设置数据位*/
     /*数据位选择*/   
     switch(nBits)
     {
         case 7:
             newttys1.c_cflag |=CS7;
             break;
         case 8:
             newttys1.c_cflag |=CS8;
             break;
     }
     /*设置奇偶校验位*/
     switch( nEvent )
     {
         case '0':  /*奇校验*/
             newttys1.c_cflag |= PARENB;/*开启奇偶校验*/
             newttys1.c_iflag |= (INPCK | ISTRIP);/*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
             newttys1.c_cflag |= PARODD;/*启用奇校验(默认为偶校验)*/
             break;
         case 'E':/*偶校验*/
             newttys1.c_cflag |= PARENB; /*开启奇偶校验  */
             newttys1.c_iflag |= ( INPCK | ISTRIP);/*打开输入奇偶校验并去除字符第八个比特*/
             newttys1.c_cflag &= ~PARODD;/*启用偶校验*/
             break;
         case 'N': /*无奇偶校验*/
             newttys1.c_cflag &= ~PARENB;
             break;
     }
     /*设置波特率*/
    switch( nSpeed )  
    {
        case 2400:
            cfsetispeed(&newttys1, B2400);
            cfsetospeed(&newttys1, B2400);
            break;
        case 4800:
            cfsetispeed(&newttys1, B4800);
            cfsetospeed(&newttys1, B4800);
            break;
        case 9600:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
        case 115200:
            cfsetispeed(&newttys1, B115200);
            cfsetospeed(&newttys1, B115200);
            break;
        default:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
    }
     /*设置停止位*/
    if( nStop == 1)/*设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB*/
    {
        newttys1.c_cflag &= ~CSTOPB;/*默认为一位停止位； */
    }
    else if( nStop == 2)
    {
        newttys1.c_cflag |= CSTOPB;/*CSTOPB表示送两位停止位*/
    }

    /*设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时*/
   // newttys1.c_cc[VTIME] = 0;/*非规范模式读取时的超时时间；*/
    //newttys1.c_cc[VMIN]  = 1; /*非规范模式读取时的最小字符数*/
    tcflush(fd ,TCIFLUSH);/*tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

     /*激活配置使其生效*/
    if((tcsetattr( fd, TCSANOW,&newttys1))!=0)
    {
        perror("com set error");
        return -1;
    }

    return 0;
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
	head.psize = 1024;
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
	
	for(i = 1; i <=head.counts;i ++)
	{
		memset(p,0,sizeof(head) + head.psize); // 
		
		
		//printf(" >>>>>>> image  name %s  \n",head.name);
		
		if(rlen >=  psize)
		{
			#if 1
			MD5_code((unsigned char *)head.MD5,&olen,(unsigned char *)buffer,head.psize);
			printf("packnum %d,md5 : %s ,md5 len %d\n",head.num,head.MD5,olen);
			
			//memset();
			memcpy(p,&head,sizeof(head));
			
		
			printf(" >>>>>>>  packet num  %d ,tmp %x,tmp + size %p ,buffer %p,l_len %d ,psize %d,head size %d \n",i,tmp,tmp+size,buffer,buffer - tmp,head.psize,sizeof(head));
			memcpy(p + sizeof(head),buffer,head.psize);
			
		
			
			tatol = write(fd,p,head.psize + sizeof(head)); // send body
			tcflush(fd, TCIOFLUSH);
			//usleep(20*1000);
		
			buffer = tmp + head.psize * i;
			rlen = size - i* head.psize;
			printf("Uart send ： %d bytes ,left rlen %d , \n",tatol,rlen);
			#else
			retval = write(fd,buffer,head.psize);
			buffer += retval;
			tatol +=retval;
			rlen -= retval;
			#endif
		}
		else
		{	
			#if 1
			head.psize = rlen; // left data
	
			MD5_code((unsigned char *)head.MD5,&olen,(unsigned char *)buffer,head.psize);
			printf("packnum %d,md5 : %s , olen %d ,rlen %d\n",head.num,head.MD5,olen,head.psize);

			memcpy(p,&head,sizeof(head));
			//if(buffer - tmp >= head.psize)
			//{
			memcpy(p+sizeof(head),buffer,head.psize);
			//}
			
			tatol = write(fd,p,head.psize + sizeof(head)); // send body
			tcflush(fd, TCIOFLUSH);
			#else
			
			retval = write(fd,buffer,rlen);
			printf("retval %d,rlen %d \n",retval,rlen);
			#endif
		
		}
		
		
		usleep(20*1000);
		
			
		if(retval >= 0)
		{
			printf("packet num : %d,packetsize %d ,tatol %d,rlen %d\n",head.num,head.psize,tatol,rlen);
			head.num ++;
		}
		//Todo recv
		while(1)
		{
			if(readTimeOut(2000,fd) > 0)
			{
				retval = read(fd,&headb,sizeof(headb));
				if(retval < 0) printf("Uart recv failed \n");
			}
		
			#if 1
			if(memcmp(headb.magic,magic,4) == 0)
			{
				//printf("headb.MD5 %s \n",headb.MD5);
				if(strncmp(headb.MD5,"BBBB",4) == 0)
				{
						printf(">>>>>>>>> ####### Recv packet id %d,packet nums %d ,packet size %d\n",headb.num,headb.counts,headb.psize);
						
						retval = write(fd,p,headb.psize + sizeof(Header)); //  retry send buffer,
						if(retval < 0) printf("Re send  failed 222 \n");
				}
				else if(strncmp(headb.MD5,"AAAA",4) == 0) // last one packet retry,
				{
					printf("&&&&&& ##### Not wait,send image %s \n",headb.name);
					break;
				}
			}
			#endif
		}
	}
	return size;
}


int Uart_init(char * dev,int nSpeed,int nBits,char nEvent,int nStop)
{
	//int fd = -1;
	int retval = -1;
	
	if(uart_fd_ < 0)
		uart_fd_ =  open(dev, O_RDWR | O_NOCTTY);
	if(uart_fd_ < 0) { printf("error open uart \n"); return -1;}
	
	//set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
	retval = set_serial(uart_fd_,nSpeed,nBits,nEvent,nStop);
	if(uart_fd) { fcntl(uart_fd, F_SETFL, 0); }
	return retval;
}

#define S "AAAA"
#define F "BBBB"
int main(int argc,const char * argv[])
{
	int retval = -1;
	
	char magic[4]={0xff,0xff,0xff,0xff};
	int psize = 1024;
	int fd = -1;
	FILE * fp = NULL;

	if(argc < 3)
	{
		printf("usage ： %s file buarate",argv[0]);
		return -1;
	}
	
	fd = Uart_init("/dev/ttyUSB0",GetBaudrate(atoi(argv[2])),8,'N',1);
	if(fd < 0) printf("open failed \n");
	
	
}
