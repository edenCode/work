#ifndef _FPGA_H
#define _FPGA_H

#include <user/qWorks.h>

/* Functions */

extern int Fpga_Open( void );
extern int Fpga_Close( int hndl );

extern int Fpga_Load( int hndl, char* name );
extern int Fpga_Done( int hndl );

#endif

/* End of file */

