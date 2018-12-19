#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <termios.h>
#include <stropts.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>



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
int main()
{
	int fd = -1;
	int retval = -1;
	char a = 0x01;
	fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
	if(fd < 0) {printf("Failed to open \n");}
	
	retval = set_serial(fd,115200,8,'N',1);
	if(retval < 0) printf("set serial failed \n");
	
	while(1)
	{
		retval = write(fd,&a,1);
		usleep(10*1000);
		a = 0x00;
		retval = read(fd,&a,1);
		printf(">>>>> a %#x \n",a);
	}
	
	return 0;
}