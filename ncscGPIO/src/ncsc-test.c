#include <ncsc.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*---------
 Main entry 
 ----------*/
int main( int argc, char **argv )
{
	int enable=0, fd;
	char ch;
	fd = open("/dev/ncsc", O_RDWR);
	if (fd<0) {
		printf("Open NCSC FAILED\n");
		return 1;
	}
	printf("Open NCSC OK\n");
	/* Never return */
	while (1) {
		ch = getchar();
		switch (ch) {
			case 'q':
				close(fd);
				return 0;
			break;
			case 'e':
				if (ioctl(fd, NCSC_IOCTL_CMD_MAKE(NCSC_CMD_ENABLE), &enable)<0)
					printf("NCSC_CMD_ENABLE(%d) FAILED\n", enable);
				enable = !enable;
			break;
			default:
			break;
		}
	}

    return 0;
}

/* End of file */

