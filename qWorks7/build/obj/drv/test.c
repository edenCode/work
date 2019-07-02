#include "uart_send.h"
#incude "uart.h"
#include "file.h"

#define N "/opt/sys/1.jpg"
int main()
{
	FILE * fp = NULL;
	int size = -1;
	char buffer[2 * 1024* 1024];
	int retval = -1;
	
	fp = fopen(N,"r + b");
	if(!fp) printf("failed \n");
	
	size = fread(buffer,1,2*1024,fp);
	retval = sendImageByUart(N,buffer,size);
	
	return 0;
}