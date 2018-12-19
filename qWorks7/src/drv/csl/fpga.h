#ifndef	_CSL_FPGA_H_
#define	_CSL_FPGA_H_

#include <qWorks.h>

/*
// FPGA Data size
*/
#define CSL_FPGA_DATA_SZ 16

/*
// Ioctls
*/
#define CSL_FPGA_CMD_START    (0x00) /* Start program */
#define CSL_FPGA_CMD_STOP     (0x01) /* End program */

#define CSL_FPGA_CMD_LOAD     (0x10) /* Load FPGA file */
#define CSL_FPGA_CMD_DONE     (0x11) /* Check if FPGA is DONE */

/*
// FPGA object. 
*/
typedef struct CSL_FPGA_OBJ
{
#ifdef __KERNEL__
	int dummy; /* Dummy */
#else
    int fd;
#endif
} CSL_FPGA_OBJ;

typedef struct CSL_FPGA_OBJ* CSL_FPGA_HANDLE;

/* functions declarations */

extern int Csl_FpgaInit ( CSL_FPGA_HANDLE hndl );
extern int Csl_FpgaExit ( CSL_FPGA_HANDLE hndl );
extern int Csl_FpgaOpen ( CSL_FPGA_HANDLE hndl );
extern int Csl_FpgaClose( CSL_FPGA_HANDLE hndl );
extern int Csl_FpgaIoctl( CSL_FPGA_HANDLE hndl, int cmd, void *params );

#endif

/* End of file */
