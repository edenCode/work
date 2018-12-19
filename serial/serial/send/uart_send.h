#include "stdio.h"
#include <sys/types.h>

int send_packet(int fd,char * file,char * buffer,int size); // 分包发一张图片
//int sendImageByUart(char * file,char * buffer,int size);
int Uart_init(char * dev,int nSpeed,int nBits,char nEvent,int nStop);