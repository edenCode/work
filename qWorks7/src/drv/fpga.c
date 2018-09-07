#include <stdio.h>
#include <string.h>
#include <fpga.h>
#include <drvLib.h>
#include "csl/csl.h"
#include "csl/fpga.h"

/*
// Fpga_Open - Open FPGA Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Fpga_Open( void )
{
	return Drv_Open(CSL_MODULE_FPGA);
}

/*
// Fpga_Close - Close FPGA Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Fpga_Close( int hndl )
{
	return Drv_Close(hndl);
}

/*
// Fpga_Load - Program PFGA file.
//
// RETURNS: 0 success, -1 if failed.
*/
int Fpga_Load( int hndl, char* name )
{
	char buf[CSL_FPGA_DATA_SZ];
	FILE *fp = NULL;
	
	/* Start Program */
	if (Drv_Ioctl(hndl, CSL_FPGA_CMD_START, NULL))
	{
		return -1;
	}
	/* Open updated file */
	fp = fopen(name, "r");
    if (fp == NULL) 
    {
		return -2;
	}
	/* Send all data to FPGA */
    while (!feof(fp))
    {
    	memset(buf, 0, CSL_FPGA_DATA_SZ);
    	fread(buf, CSL_FPGA_DATA_SZ, 1, fp);
		if (Drv_Ioctl(hndl, CSL_FPGA_CMD_LOAD, buf))
		{
			fclose(fp);
			return -3;
		}
    }
    fclose(fp);
    /* End FPGA update */
	if (Drv_Ioctl(hndl, CSL_FPGA_CMD_STOP, NULL))
	{
		return -4;
	}
	return 0;
}

/*
// Fpga_Done - Check if FPGA Object is done.
//
// RETURNS: 0 success, -1 if failed.
*/
int Fpga_Done( int hndl )
{
	int done = 0;
	
	/* End FPGA update */
	if (Drv_Ioctl(hndl, CSL_FPGA_CMD_DONE, &done))
	{
		return -1;
	}	
	return done;
}

/* End of file */

