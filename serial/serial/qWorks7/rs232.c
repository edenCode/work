#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <drv/uart.h>

/*
// Main entry 
*/
int main( int argc, char **argv )
{
	char tx[20] = {0x12, 0x34, 0x56, 0x78,0x12, 0x34, 0x56, 0x78,0x12, 0x34, 0x56, 0x78,0x12, 0x34, 0x56, 0x78,0x12, 0x34,0x12, 0x34};
	char rx[20] = {0,};
    int ch, hUart = -1;
	
	hUart = Uart_Open("/dev/ttyUSB0", NULL);
	if (hUart<0) {
		printf("[ FAILED ]\n");
		return 1;
	}
	/* Never return */
	while (1) {
		ch = getchar();
		switch (ch) {
			case 'q':
				Uart_Close(hUart);
				return 0;
			break;
			case 'w':
				if (write(hUart, tx, 4)<0) {
					printf("Uart_Write FAILED.\n");
				} else {
					printf("RS232 TX %02X-%02X-%02X-%02X \n", tx[0], tx[1], tx[2], tx[3]);
				}
			break;
			case 'r':
				if (read(hUart, rx, 4)<0) {
					printf("Uart_Read FAILED.\n");
				} else {
					printf("RS232 RX %02X-%02X-%02X-%02X \n", rx[0], rx[1], rx[2], rx[3]);
				}
			break;
			default:
			break;
		}
	}
	
	return 0;
}

/* End of file */

