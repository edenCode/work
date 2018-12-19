#include "uart.h"
#include "string.h"

#define DEV_NAME "/dev/ttyUSB0"

typedef enum{
	MAGICHead =0xFFFFFFFF,
}Magic_head;
typedef struct Header{
	char *magic ;
	int packetsize;
	int count;
	int num;
}Header;

#define MAGIC 0xFFFFFFFF
int main()
{
	int retval = -1;
	char magic[4]={0xff,0xff,0xff,0xff};
	Header head,head1;
	int p_len = 1024;
	int size = 1024;
	int fd = -1;
	fd = Uart_Open (DEV_NAME,NULL);
	if(fd < 0) { printf("error open uart \n")  return -1;};
	
	head.magic = magic;
	head.packetsize = size;
	head.count = size / p_len;
	head.num = 1;
	printf("Head size %d \n",sizeof(head));
	while(1){
		
		retval = Uart_SendData(fd,&head,sizeof(head));
		printf("Uart send ： %d bytes \n",retval);
		
		retval = Uart_RecvData(fd,&head1,sizeof(head1));
		printf("Uart recv retval : %d \n",retval);
		if(retval > 0)
		{
			printf("head magic %s \n",head1.magic);
			if(strncmp(head1.magic,magic,4) == 0)
			{
				printf("Uart recv magic,check it \n");
			}
			printf("head packetsize : %d \n",head1.packetsize);
			printf("head count ; %d \n",head1.count);
			printf("head num : %d \n",head1.num);
		}
	}
	
	
}