#ifndef _I2C_LIB_H_
#define _I2C_LIB_H_

#include <ufx.h>

/* function declarations */

extern int I2c_Open( int idx );
extern int I2c_Close( int hndl );
extern int I2c_SendData( int hndl, int addr, char* buf, int len );
extern int I2c_RecvData( int hndl, int addr, char* buf, int len );

#endif 

/* End of file */
