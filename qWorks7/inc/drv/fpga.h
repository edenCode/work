#ifndef _FPGA_H
#define _FPGA_H

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

int Fpga_Open( void );
int Fpga_Close( int hndl );

int Fpga_Load( int hndl, char* name );
int Fpga_Done( int hndl );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

