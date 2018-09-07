#include "stdio.h"

int send_packet(int fd,char * buffer,int size); // 分包发一张图片
int sendImageByUart(char * file,char * buffer,int size);