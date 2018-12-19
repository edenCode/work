#include "uart.h"

#define DEV "/dev/ttyS"
int main()
{
	int i = 0;
	int j = 0;
	int fd = -1;
	char name[256];
	UART_ATTRS attrs;
	int retval = 0;
	char data[10]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char buf[10];
	attrs.rate   = UART_BAUDRATE_B9600;
	attrs.size   = UART_DATASIZE_DEFAULT;
	attrs.stop   = UART_STOPBIT_DEFAULT;
	attrs.parity = UART_PARITY_DEFAULT;
	
	for(i = 0;i < 191; i ++)
	{
		sprintf(name,"/dev/ttyS%d",i);
		fd = Uart_Open (name,&attrs);
		if(fd){
			retval = write(fd,data,10);
			printf("retval : %d \n",retval);
			retval = read(fd,buf,10);
			if(retval < 0 && strncmp(buf,data,10) != 0)
			{
				Uart_Close(fd);
				continue;
			}
			for(j = 0; j < 10;j ++)
			{
				printf(" %#x ");
			}
			Uart_Close(fd);
			printf("\n");
		}
	}
}