#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include <drvLib.h>
#include "csl/csl.h"

/*
// Drv_Open - Open device by module id.
//
// RETURNS: 0 if success, !0 if failed. 
*/
int Drv_Open( int module )
{
	char deviceName[20];

	if (module>=CSL_MODULE_MAX) return -1;
	
	sprintf(deviceName, "/dev/%s%d", CSL_DRV_NAME, module);
	
	return open(deviceName, O_RDWR);
}

/*
// Drv_Close - Close device module.
//
// RETURNS: 0 if success, !0 if failed. 
*/ 
int Drv_Close( int fd )
{
	return close(fd);
}

/*
// Drv_Close - Control device module.
//
// RETURNS: 0 if success, !0 if failed. 
*/ 
int Drv_Ioctl( int fd, int cmd, void *args )
{
	cmd = CSL_IOCTL_CMD_MAKE(cmd);

	return ioctl(fd, cmd, args);
}

/* End of file */
