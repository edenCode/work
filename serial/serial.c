#include "serial.h"
#include<stdio.h>		//��׼�����������
#include<stdlib.h>		//��׼�����ⶨ��
#include<unistd.h>		//Unix ��׼��������
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>		//������ƶ���
#include<termios.h>		//PPSIX �ն˿��ƶ���
#include<errno.h>		//����ö���
#include<string.h>


int speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200 };

int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200};

void SetSpeed(int fd, int speed)
{
	unsigned int i;
	int   status;
	struct termios Opt;
	tcgetattr(fd, &Opt);

	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if  (speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if  (status != 0)
					perror("tcsetattr fd1");
			return;
		}
		tcflush(fd,TCIOFLUSH);
	}
}

/**
*@brief   ���ô�������λ��ֹͣλ��Ч��λ
*@param  fd     ����  int  �򿪵Ĵ����ļ����*
*@param  databits ����  int ����λ   ȡֵ Ϊ 7 ����8*
*@param  stopbits ����  int ֹͣλ   ȡֵΪ 1 ����2*
*@param  parity  ����  int  Ч������ ȡֵΪN,E,O,,S
*/
int SetParity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;

	if( tcgetattr( fd,&options)  !=  0)
	{
		perror("SetupSerial 1");
		return -1;
	}
	options.c_cflag &= ~CSIZE;

	switch (databits) /*��������λ��*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");
			return -1;
	}
	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);  /* ����Ϊ��Ч��*/
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported parity\n");
			return -1;
	}
	/* ����ֹͣλ*/
	switch (stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return -1;
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 0; // 15 seconds
	options.c_cc[VMIN] = 1;

	//raw mode
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	options.c_oflag &= ~OPOST;
	//0x11 receive
	options.c_iflag &= ~(ICRNL | IXON);

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return -1;
	}

	return 0;
}

int OpenDev(const char *Dev)
{
	int	fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY );
	if (-1 == fd)
	{ /*��������λ��*/
		perror("Can't Open Serial Port");
		return -1;
	}
	fcntl(fd , F_SETFL , 0);

	return fd;
}


