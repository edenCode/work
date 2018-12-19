#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stropts.h>
#include <stdio.h>
#include <termios.h>
#include <uart.h>
#include <osFile.h>


typedef struct Header{
	char magic[4]; // start code,eg : 0xFFFFFFFF
	char name[128];
	int size; //	data size 
	int psize; //  packet size
	int counts; //  size / psize , if(lsize == 0) size = count * psize;
	int num; //  size % psize， if(lszie != 0) size = count * psize + lsize;
	char MD5[MD5_SIZE];
}Header;

/*
// GetBaudrate - Convert readable to real baud-rate. 
// 
// RETURNS: the baud-rate. 
*/
static int GetBaudrate( UART_BAUDRATE baudrate )
{
	int rate = B9600;

	switch (baudrate) {
		case UART_BAUDRATE_B1200  : rate = B1200; break;
		case UART_BAUDRATE_B1800  : rate = B1800; break;
		case UART_BAUDRATE_B2400  : rate = B2400; break;
		case UART_BAUDRATE_B4800  : rate = B4800; break;
		case UART_BAUDRATE_B9600  : rate = B9600; break;
		case UART_BAUDRATE_B19200 : rate = B19200; break;
		case UART_BAUDRATE_B38400 : rate = B38400; break;
		case UART_BAUDRATE_B57600 : rate = B57600; break;
		case UART_BAUDRATE_B115200: rate = B115200; break;
		default: break;
	}
	return rate;
}

/*
// Uart_Open - Create Uart device with specified attribute. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_Open( char* name, UART_ATTRS *data )
{
	int fd;
	UART_ATTRS attrs;
	struct termios opts;
    int tmp, retval = 0;

    if (data==NULL) {
		/* Use default attributes */
		attrs.rate   = UART_BAUDRATE_DEFAULT;
		attrs.size   = UART_DATASIZE_DEFAULT;
		attrs.stop   = UART_STOPBIT_DEFAULT;
		attrs.parity = UART_PARITY_DEFAULT;
	} else {
		attrs = *data;
	}

    /* 1. Open tty device */
	fd = open(name, O_RDWR | O_NOCTTY);
	if (fd<0) return -1;
	
    /* 2. Get the tty attributes */
	if (tcgetattr(fd, &opts)) {
		close(fd);
		return -1;
    }
	/* 3. Choose raw input/output */
	opts.c_lflag &= ~(ICANON | ECHO  | ECHOE | ISIG);
	opts.c_iflag &= ~(INLCR  | IGNCR | ICRNL);
	opts.c_iflag &= ~(IXON   | IXOFF | IXANY);
	opts.c_oflag &= ~(ONLCR  | OCRNL);
	opts.c_oflag &= ~OPOST;
	
	/* 4. Enable the receiver and set local mode... */
	opts.c_cflag |= (CLOCAL | CREAD);
	
	/* 5. Set baud-rate of tty */
	tmp     = GetBaudrate(attrs.rate);
	retval  = cfsetispeed(&opts, tmp);
	retval |= cfsetospeed(&opts, tmp);
	if (retval) {
        close(fd);
		return -1;
    }
    /* 6. Set data size of tty, 5 - 8 bits */
    switch (attrs.size) {
		case UART_DATASIZE_5BITS : tmp = CS5; break;
		case UART_DATASIZE_6BITS : tmp = CS6; break;
		case UART_DATASIZE_7BITS : tmp = CS7; break;
		case UART_DATASIZE_8BITS : tmp = CS8; break;
		default: tmp = CS8; break;
	}
	opts.c_cflag &= ~CSIZE;
	opts.c_cflag |= tmp;
	
	/* 7. Set parity of tty */
	switch (attrs.parity) {
		case UART_PARITY_NONE: /* none */
			opts.c_cflag &= ~PARENB;
			opts.c_iflag &= ~INPCK; 
		break;
		case UART_PARITY_ODD: /* odd */
			opts.c_cflag |= PARENB;
			opts.c_cflag |= PARODD;
			/* check parity and skip it */
			opts.c_iflag |= (INPCK | ISTRIP);
		break;
		case UART_PARITY_EVEN: /* even */
			opts.c_cflag |= PARENB;
			opts.c_cflag &= ~PARODD;
			/* check parity and skip it */
			opts.c_iflag |= (INPCK | ISTRIP); 
		break;
		default:
			opts.c_cflag &= ~PARENB;
			opts.c_iflag &= ~INPCK; 
		break;
	}
	/* 8. Set stop bit(s) of tty */
	if (attrs.stop) 
		opts.c_cflag |= CSTOPB;
	else 
		opts.c_cflag &= ~CSTOPB;
	tcflush(fd, TCIOFLUSH);
    /* 9. Set attribute of tty */
	if (tcsetattr(fd, TCSANOW, &opts)) {
		close(fd);
		return -1;
	}
	fcntl(fd, F_SETFL, 0);

	return fd;	
}

/*
// Uart_Close - Delete UART device. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_Close( int fd )
{
	return close(fd);
}

/*
// Uart_SetBaudRate - Set uart baudrate. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_SetBaudRate( int fd, UART_BAUDRATE rate )
{
	struct termios opts;
    int tmp, retval = 0;

    /* Get the tty attributes */
	if (tcgetattr(fd, &opts)) return -1;
	
	/* Set baud-rate of tty */
	tmp     = GetBaudrate(rate);
	retval  = cfsetispeed(&opts, tmp);
	retval |= cfsetospeed(&opts, tmp);
	if (retval) return -1;

    /* Set attribute of tty */
	if (tcsetattr(fd, TCSANOW, &opts)) return -1;

	return 0;	
}

/*
// Uart_SetDataSize - Set data size. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_SetDataSize( int fd, UART_DATASIZE size )
{
	struct termios opts;
	int tmp = 0;

    /* Get the tty attributes */
	if (tcgetattr(fd, &opts)) return -1;

    /* Set data size of tty, 5 - 8 bits */
    switch (size) {
		case UART_DATASIZE_5BITS : tmp = CS5; break;
		case UART_DATASIZE_6BITS : tmp = CS6; break;
		case UART_DATASIZE_7BITS : tmp = CS7; break;
		case UART_DATASIZE_8BITS : tmp = CS8; break;
		default: tmp = CS8; break;
	}
	opts.c_cflag &= ~CSIZE;
	opts.c_cflag |= tmp;
	
    /* Set attribute of tty */
	if (tcsetattr(fd, TCSANOW, &opts)) return -1;

	return 0;	
}

/*
// Uart_SetStopBits - Set uart stop bits. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_SetStopBits( int fd, UART_STOPBITS bits )
{
	struct termios opts;

    /* Get the tty attributes */
	if (tcgetattr(fd, &opts)) return -1;

	/* Set stop bit(s) of tty */
	if (bits) 
		opts.c_cflag |= CSTOPB;
	else 
		opts.c_cflag &= ~CSTOPB;

    /* Set attribute of tty */
	if (tcsetattr(fd, TCSANOW, &opts)) return -1;

	return 0;	
}

/*
// Uart_SetParity - Set uart Parity. 
// 
// RETURNS: 0:success, -1:failed.
*/
int Uart_SetParity( int fd, UART_PARITY parity )
{
	struct termios opts;
	
    /* Get the tty attributes */
	if (tcgetattr(fd, &opts)) return -1;
	
	/* Set parity of tty */
	switch (parity) {
		case UART_PARITY_NONE: /* none */
			opts.c_cflag &= ~PARENB;
			opts.c_iflag &= ~INPCK; 
		break;
		case UART_PARITY_ODD: /* odd */
			opts.c_cflag |= PARENB;
			opts.c_cflag |= PARODD;
			/* check parity and skip it */
			opts.c_iflag |= (INPCK | ISTRIP);
		break;
		case UART_PARITY_EVEN: /* even */
			opts.c_cflag |= PARENB;
			opts.c_cflag &= ~PARODD;
			/* check parity and skip it */
			opts.c_iflag |= (INPCK | ISTRIP); 
		break;
		default:
			opts.c_cflag &= ~PARENB;
			opts.c_iflag &= ~INPCK; 
		break;
	}
	if (tcsetattr(fd, TCSANOW, &opts)) return -1;

	return 0;	
}

/*
// Uart_FlushData - Flush data buffer. 
//  
// RETURNS: Bytes written, -1:failed.
*/
int Uart_FlushData( int fd )
{
	return tcflush(fd, TCIOFLUSH);
}

/*
// Uart_SendData - Write data to UART port. 
//  
// RETURNS: Bytes written, -1:failed.
*/
int Uart_SendData( int fd, char* buf, int len )
{
	return OSFile_WriteAll(fd, buf, len);
}

/*
// Uart_RecvData - Read data to UART port. 
//  
// RETURNS: Bytes written, -1:failed.
*/
int Uart_RecvData( int fd, char* buf, int len )
{
	return OSFile_ReadAll(fd, buf, len);
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

/* End of file */

