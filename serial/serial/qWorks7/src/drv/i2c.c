#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <pthread.h>
#include <osError.h>
#include <osFile.h>
#include <i2c.h>

/*
// I2C name under Linux
*/
// static char* i2cDevice[] = 
// {
	// "/dev/i2c-1", /* I2C0 */
	// "/dev/i2c-2", /* I2C1 */
	// "/dev/i2c-3"  /* I2C2 */
// };

/*
// i2c master command, moved from /linux/i2c-dev.h 
*/
#define I2C_SLAVE 0x0703 /* Change slave address */

/*
// I2c_Open - Create I2C Object.
//
// RETURNS: Handle of I2C Object, NULL if failed.
*/
int I2c_Open( char* name )
{
	int hndl;
	
	hndl = open(name, O_RDWR);
    if (hndl<0) {
		OSError_Set(errno);
		return (-1);
	}
	return hndl;
}

/*
// I2c_Close - Delete I2C Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int I2c_Close( int hndl )
{
	return close(hndl);
}

/*
// I2c_Send - Send data to I2C slave device.
//
// RETURNS: 0 success, -1 if failed.
*/
int I2c_Send( int hndl, int addr, char* buf, int len )
{
	if (ioctl(hndl, I2C_SLAVE, addr)<0) {
		OSError_Set(errno);
        return -1;
    }
	/* Write to I2C */
	if (OSFile_WriteAll(hndl, buf, len)<0) {
		OSError_Set(errno);
        return -1;
	}
	return 0;
}

/*
// I2c_Receive - Receive data to I2C slave device.
//
// RETURNS: 0 success, -1 if failed.
*/
int I2c_Receive( int hndl, int addr, char* buf, int len )
{
	if (ioctl(hndl, I2C_SLAVE, addr)<0) {
		OSError_Set(errno);
        return -1;
    }
	/* Write to I2C */
	if (OSFile_ReadAll(hndl, buf, len)<0) {
		OSError_Set(errno);
        return -1;
	}
	return 0;
}

/* End of file */

