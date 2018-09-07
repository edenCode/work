#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>

/* function declarations */

int I2c_Open( char* name );
int I2c_Close( int hndl );

int I2c_Send( int hndl, int addr, char* buf, int len );
int I2c_Receive( int hndl, int addr, char* buf, int len );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
