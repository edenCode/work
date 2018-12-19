#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stropts.h>
#include <termios.h>
#include "md5.h"

//#define __TEST__
//#define __MD5__  

#define S "AAAA"
#define F "BBBB"
#define MAGIC "MAXVSION"
#define MD5_SIZE 32
//int uart_fd_ = -1;

//MAXVSION
#pragma pack (1)
typedef struct Header{
	char magic[9]; // start code,eg : 0xFFFFFFFF
	char name[32];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
//#ifdef __MD5__
	char MD5[MD5_SIZE];
//#endif
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

int ReadPacket(int fd,char * buffer,int len)
{
	int i = 0;
	int retval = 0;
	int size;
	char ch;
	
	for(i = 0;i < len;i ++)
	{
		retval = read(fd,&ch,1);
		if(retval <= 0) break;
		size += retval;
		buffer[i] = ch;
	}
	
	return retval;
}

int send_packet(int fd,char * name,char * buffer,int size) // 分包发一张图片
{
	//char magic[4]={0xff,0xff,0xff,0xff};
	Header head,headb;
	int i = 0;
	int rlen = 0;
	int retval = 0;
	int psize;
	char * tmp = buffer;
	int tatol = 0;
	char * p = NULL;
	int olen = 32;
	
	memset(head.magic,'\0',32);
	memcpy(head.magic,MAGIC,strlen(MAGIC));
	printf("start head magic %s ,MAGIC %s\n",head.magic,MAGIC);
	//head.magic = MAGIC;
	head.psize = 1024*2;
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
		//printf("packet len %d \n",head.psize + sizeof(head));
	}
	
	for(i = 1; i <=head.counts;i ++)
	{
		memset(p,0,sizeof(head) + head.psize); 
		
		if(rlen >=  psize)
		{
		
			#ifdef __MD5__
				MD5_code((unsigned char *)head.MD5,&olen,(unsigned char *)buffer,head.psize);
				printf("packnum %d,md5 : %s ,md5 len %d,psize %d,\n",head.num,head.MD5,olen,head.psize);
			#endif
			
			memcpy(p,&head,sizeof(head));
			memcpy(p + sizeof(head),buffer,head.psize);

			printf(" >>>>>>>  packet num  %d ,l_len %d ,psize %d,head size %d \n",i,buffer - tmp,head.psize,sizeof(head));
			tatol += write(fd,p,(head.psize + sizeof(head))); // send body
		
			buffer = tmp + head.psize * i;
			rlen = size - i* head.psize;

		}
		else
		{	
		
			head.psize = rlen; // left data
			
			#ifdef __MD5__
				MD5_code((unsigned char *)head.MD5,&olen,(unsigned char *)buffer,head.psize);
				printf("packnum %d,md5 : %s , olen %d ,rlen %d\n",head.num,head.MD5,olen,head.psize);
			#endif
			memcpy(p,&head,sizeof(head));
			memcpy(p+sizeof(head),buffer,head.psize);
	
			
			tatol += write(fd,p,head.psize + sizeof(head)); // send body
			rlen -= rlen;
			//tcflush(fd, TCIOFLUSH);
		}
		
		
		//usleep(20*1000);
		
			
		if(retval >= 0)
		{
			printf("packet num : %d,head size %d,packetsize %d ,filesize %d,tatol %d,rlen %d\n",head.num,sizeof(head),head.psize,size,tatol,rlen);
			head.num ++;
		}
		//Todo recv
		while(1)
		{
			memset(&headb,0,sizeof(headb));
			if(readTimeOut(2000,fd) > 0)
			{
				usleep(20 * 1000);
				retval = ReadPacket(fd,(char *)&headb,sizeof(headb));
				if(retval < 0) printf("Uart recv failed \n");
			
				
				if(retval > 0 && retval == sizeof(headb))
				{
				
					if(strncmp(headb.magic,MAGIC,8) == 0)
					{
						//#ifdef __MD5__
							if(strncmp(headb.MD5,"BBBB",4) == 0)
							{
								printf("<<<<<<<<< Packet error ,resend >>>>>>>>> ####### Recv packet id %d,packet nums %d ,packet size %d\n",headb.num,headb.counts,headb.psize);
						
								retval = write(fd,p,headb.psize + sizeof(Header)); //  retry send buffer,
								if(retval < 0) printf("Re send  failed 222 \n");
							}
							else if(strncmp(headb.MD5,"AAAA",4) == 0) // last one packet retry,
							{
								printf("&&&&&& ##### Not wait,send image %s \n",headb.name);
								break;
							}
	
					}
					else
					{
						printf(">>>> error MAGIC len %d, MAGIC %s, headb.magic %s \n",strlen(MAGIC),MAGIC,headb.magic);
					
						retval = write(fd,p,headb.psize + sizeof(Header)); //  retry send buffer,
						if(retval < 0) printf("Re send  failed 222 \n");
					}
				
				}
				else
				{
					printf(">>>>> read retval %d \n",retval);
					if(retval < sizeof(headb))
					{
						
					}
		
				}
			}
			else
			{
				printf("Wait Recv Message  ... \n");
			}
		}
	}
	
	if(p) free(p);
	
	return size;
}


int Uart_init(char * dev,int nSpeed,int nBits,char nEvent,int nStop)
{
	//int fd = -1;
	int retval = -1;
	int ufd = -1;
	if(ufd < 0)
		ufd =  open(dev, O_RDWR | O_NOCTTY);
	if(ufd < 0) { printf("error open uart \n"); return -1;}
	
	//set_serial(int fd,int nSpeed,int nBits,char nEvent,int nStop)
	retval = set_serial(ufd,nSpeed,nBits,nEvent,nStop);
	if(retval < 0) {printf("set serial error \n");}
	if(ufd) { fcntl(ufd, F_SETFL, 0); }
	return ufd;
}

#ifdef __TEST__
int main(int argc,const char * argv[])
{
	int retval = -1;
	
	//char magic[4]={0xff,0xff,0xff,0xff};
	int psize = 1024;
	int fd = -1;
	FILE * fp = NULL;
	int size = -1;
	char * buffer = NULL;
	//char buffer[];
	if(argc < 3)
	{
		printf("usage : %s file buarate\n",argv[0]);
		return -1;
	}
	printf("file name : %s ,Baudrate : %d \n",argv[1],GetBaudrate(atoi(argv[2])));
	fd = Uart_init("/dev/ttyUSB0",GetBaudrate(atoi(argv[2])),8,'N',1);
	if(fd < 0) printf("open failed \n");
	
	fp = fopen(argv[1],"r+b");
	if(!fp) printf("open %s failed \n",argv[1]);
	
	fseek(fp,0,SEEK_SET);
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	rewind(fp);
	if(!buffer) buffer = (char *) malloc(size); 
	fread(buffer,1,size,fp);
	rewind(fp);
	fclose(fp);
	printf(">>>>> file size %d \n",size);
	retval = send_packet(fd,(char *)argv[1],buffer,size);
	printf("send file size %d \n",retval);
	
	return 0;
}
#else
#define CAM_INSTRUCTION_START_BYTE 0xAA                              //相机指令格式开始字节
#define  BASIC_CHECK_TYPE 0xFE                                                 //基板返回数据校验位

int printn(char * buffer,int len)
{
	int i = 0;
	
	printf("\n\n");
	for(i = 0;i < len;i ++)
	{
		printf(" %#x ",*buffer++);
	}
	printf("\n\n");
}
int main(int argc,char * argv[])
{
	char dev[48];
	char buffer[256] = {0};
	char gps_info[48];
	char bufoffer[3] = {CAM_INSTRUCTION_START_BYTE , 0x0D, BASIC_CHECK_TYPE};
	char cmd[3] = {CAM_INSTRUCTION_START_BYTE,0x14, BASIC_CHECK_TYPE}; // get GPS time
	char cmd_s[3] = {CAM_INSTRUCTION_START_BYTE , 0x0E, BASIC_CHECK_TYPE};
	int retval = -1;
	int fd = -1;
	char * temp = NULL;
	int flag = 0;
	int count = 0;
	char * p = NULL;
	if(argc < 2)
	{
		printf("Usage: %s uartnum \n",argv[0]);
		return -1;
	}
	
	sprintf(dev,"/dev/ttyO%d",atoi(argv[1]));
	printf(">>>>>>>> open uart %s \n",dev);
	fd = Uart_init("/dev/ttyO4",GetBaudrate(8),8,'N',1);
	
	if(fd < 0) {printf("error open gps uart \n");}
	
	tcflush(fd,TCIOFLUSH);
	
	while(1)
	{
		retval = write(fd,bufoffer,sizeof(bufoffer));
		printf("write val %d  bytes \n",retval);
	
		if(readTimeOut(200,fd) > 0)
		{
			memset(buffer,0,sizeof(buffer));
			retval = read(fd,buffer,sizeof(buffer));
			printf("<< GPS info >>read  %d bytes ,read value %s ,%d\n",retval,buffer,buffer);
			printn(buffer,retval);
			
			printf("\n\n############################################################\n\n");
			
			temp = strtok(buffer,",");
			while(temp)
			{
				printf("temp : %s \n",temp);
				temp = strtok(NULL,",");
				
				if(strcmp(temp,"A") == 0)
				{
					flag = 1;
					continue;
				}
				
				if(strcmp(temp,"V") == 0)
				{
					break;
				}
					
				
				if(flag == 1) p = temp;
				count += strlen(temp);
					
				if(flag == 4)
				{
					strncpy(gps_info,p,count);
					break;
				}
					
				flag ++;
			
				
			}
			
			printf("gps_info : %s ,count %d\n",gps_info,count);
		}
		else
		{
			printf("time out ... flush IO ...\n");
			tcflush(fd,TCIOFLUSH);
		}
		
		usleep(1000*500);
		
		retval = write(fd,cmd_s,sizeof(cmd_s));
		printf("write val %d  bytes \n",retval);
	
		if(readTimeOut(200,fd) > 0)
		{
			memset(buffer,0,sizeof(buffer));
			retval = read(fd,buffer,sizeof(buffer));
			printf("<< charge info >>read  %d bytes ,read charge value %s ,%d \n",retval,buffer,buffer);
			printn(buffer,retval);

		}
		else
		{
			printf("time out ... flush IO ...\n");
			tcflush(fd,TCIOFLUSH);
		}
		
	}
}
#endif