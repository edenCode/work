#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <stropts.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include "md5.h"

#define MD5_SIZE 32
#define S "AAAA"
#define F "BBBB"
#define MAGIC "MAXVSION"

#pragma pack (1)
typedef struct Header{
	 char magic[9];// start code,eg : 0xFFFFFFFF
	char name[32];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
	char MD5[MD5_SIZE];
}Header;
#pragma pack ()

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

int set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
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
    //  newttys1.c_cc[VTIME] = 0;/*非规范模式读取时的超时时间；*/
     // newttys1.c_cc[VMIN]  = size * 1024; /*非规范模式读取时的最小字符数*/
    tcflush(fd ,TCIFLUSH);/*tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

     /*激活配置使其生效*/
    if((tcsetattr( fd, TCSANOW,&newttys1))!=0)
    {
        perror("com set error");
        return -1;
    }

    return 0;
}

static int GetBaudrate(int brate)
{
	int rate = B9600;

	switch (brate) {
		case 0  : rate = 1200; break;
		case 1  : rate = 1800; break;
		case 2  : rate = 2400; break;
		case 3  : rate = 4800; break;
		case 4  : rate = 9600; break;
		case 5 : rate = 19200; break;
		case 6 : rate = 38400; break;
		case 7 : rate = 57600; break;
		case 8: rate = 115200; break;
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

int ReadPacket(int fd,char * buffer,int len)
{
	int i = 0;
	int retval = 0;
	char ch;
	
	for(i = 0;i < len;i ++)
	{
		retval += read(fd,&ch,1);
		buffer[i] = ch;
	}
	
	return retval;
}

int Uart_init(char * dev,int nSpeed,int nBits,char nEvent,int nStop)
{
	//int fd = -1;
	int retval = -1;
	int uart_fd_ = -1;
	if(uart_fd_ < 0)
		uart_fd_ =  open(dev, O_RDWR | O_NOCTTY);
	if(uart_fd_ < 0) { printf("error open uart \n"); return -1;}
	
	//set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
	retval = set_serial(uart_fd_,nSpeed,nBits,nEvent,nStop);
	if(uart_fd_) { fcntl(uart_fd_, F_SETFL, 0); }
	return uart_fd_;
}


int main(int argc,const char * argv[])
{
	int retval = -1;
	
	//char magic[4]={0xff,0xff,0xff,0xff};
	int psize = 200;
	int fd = -1;
	FILE * fp = NULL;
	char * file = (char *) malloc(2 * 1024 * 1024);
	Header * pHead;
	char md5[MD5_SIZE];
	int md5Len = MD5_SIZE;
	//Header head;
	if(argc < 3)
	{
		printf(" Usage : %s brate psize\n",argv[0]);
		return -1;
	}

	
	fd = Uart_init("/dev/ttyUSB0",GetBaudrate(atoi(argv[1])),8,'N',1);
	if(fd < 0) printf("open failed \n");
	
	if(atoi(argv[2]) > 0)
		psize = atoi(argv[2]);
	
	int len = sizeof(Header) + psize;
	char buffer[len];
	printf("packet size %d \n",len);
	
	while(1)
	{
		
		if(readTimeOut(200,fd) > 0)
		{
			usleep(200 * 1000);
			retval = ReadPacket(fd,buffer,len);
			//printf(">>>> retval %d \n",retval);
			if(retval > 0 && retval == len)
			{
				pHead = (Header *)buffer;
				printf("<<<< ReadPacket >>>> Retval %d,Magic %s,id %d,size %d,psize %d,counts %d,fileName %s\n",retval,pHead->magic,pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name);
				if(strncmp(pHead->magic,MAGIC,8) == 0)
				{
					
					MD5_code((unsigned char *)md5,&md5Len,(unsigned char *)buffer + sizeof(Header),pHead->psize);
					printf(">>> md5 %s \n>>> MD5 %s \n",md5,pHead->MD5);
					if(!strncmp(md5,pHead->MD5,MD5_SIZE))
					{
						memcpy(file,buffer + sizeof(Header),pHead->psize);
						file += pHead->psize;
						
						memset(pHead->MD5,'\0',MD5_SIZE);
						memcpy(pHead->MD5,S,4);
						//strncpy(pHead->MD5,S,4);
						pHead->MD5[4] = '\0';
						//printf(" << Successful send >>id %d,size %d,psize %d,counts %d,fileName %s ,MD5 %s \n",pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name,pHead->MD5);
					}
					else
					{
						//printf("33333 \n");
						memset(pHead->MD5,'\0',MD5_SIZE);
						//printf("33333  1111 \n");
						memcpy(pHead->MD5,F,4);
						//strncpy(pHead->MD5,F,4);
						//pHead->MD5[4] = '\0';
						//printf(" << Failed send MD5 >>id %d,size %d,psize %d,counts %d,fileName %s ,MD5 %s \n",pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name,pHead->MD5);
					}
				}
				else
				{
					//printf(">>>> error MAGIC len %d, MAGIC %s, headb.magic %s \n",strlen(MAGIC),MAGIC,pHead->magic);
					
					memset(pHead->MD5,'\0',MD5_SIZE);
					//printf("22222 \n");
					memcpy(pHead->MD5,F,4);
					//printf("222 1111 \n");
					//strncpy(pHead->MD5,"BBBB",4);
					//printf("222  2222 \n");
					//pHead->MD5[4] = '\0';
					//printf(" << Failed send  MAGIC >>id %d,size %d,psize %d,counts %d,fileName %s ,MD5 %s \n",pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name,pHead->MD5);
				}
			}
			else
			{
				//printf("11111 \n");
				memset(pHead->MD5,'\0',MD5_SIZE);
				//printf("1111 2222 \n");
				memcpy(pHead->MD5,F,4);
				//strncpy(pHead->MD5,F,4);
				//pHead->MD5[4] = '\0';
				//printf(" << Failed send Retval >>id %d,size %d,psize %d,counts %d,fileName %s ,MD5 %s \n",pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name,pHead->MD5);
			}
		
			retval = write(fd,(char *)pHead,sizeof(Header));
			if(retval < 0) printf("write data failed \n");
			//printf("write data ,retval %d \n",retval);
			printf("<<<< write >>>> Retval %d,Magic %s,id %d,size %d,psize %d,counts %d,fileName %s,MD5 %s\n",retval,pHead->magic,pHead->num,pHead->size,pHead->psize,pHead->counts,pHead->name,pHead->MD5);
		}
		else
		{
			printf("wait data ... \n");
		}
	}
	
}