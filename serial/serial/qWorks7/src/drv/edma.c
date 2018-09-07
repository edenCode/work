#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "csl/edmak.h"

/*
// Locals
*/
static int hEdma = -1;

/*
// Edma_Init - Init EDMA Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Init( void )
{
	char name[32];
	
	if (hEdma==-1)
	{
		sprintf(name, "/dev/%s", EDMA_DRV_NAME);
		hEdma = open(name, O_RDWR);
		if (hEdma<0) return -1;
	}
	return 0;
}

/*
// Edma_Cleanup - Exit EDMA Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Cleanup( void )
{
	if (hEdma<0) return -1;
	
	/* Close EDMA handle */
	close( hEdma );
	hEdma = -1;
	
	return 0;
}

/*
// Edma_Open - Open EDMA channel.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Open( int dmaMode, int maxTransfers )
{
	EDMA_CHANNEL param;
	int cmd = EDMA_IOCTL_CMD_MAKE(EDMA_CMD_CH_OPEN);

	if (hEdma<0) return -1;
	param.chId = -1;
	param.mode = dmaMode;
	param.maxTransfers = maxTransfers;
	if (ioctl(hEdma, cmd, &param))
		return -1;

	return param.chId;
}

/*
// Edma_Close - Open EDMA channel.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Close( int hndl )
{
	EDMA_CHANNEL param;
	int cmd = EDMA_IOCTL_CMD_MAKE(EDMA_CMD_CH_CLOSE);

	if (hEdma<0) return -1;
	param.chId = hndl;
	param.mode = 0;
	param.maxTransfers = 0;
	if (ioctl(hEdma, cmd, &param))
		return -1;
	
	return 0;
}

/*
// Edma_Copy1D - Do EDMA 1D copy.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Copy1D( int hndl, EDMA_COPY1D *prm, int count )
{
	EDMA_COPYFILL param;
	int cmd = EDMA_IOCTL_CMD_MAKE(EDMA_CMD_COPY_1D);

	if (hEdma<0) return -1;
	param.chId = hndl;
	param.count = count;
	param.unData.copy1D = *prm;
	if (ioctl(hEdma, cmd, &param))
		return -1;

	return 0;
}

/*
// Edma_Copy2D - Do EDMA 2D copy.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Copy2D( int hndl, EDMA_COPY2D *prm, int count )
{
	EDMA_COPYFILL param;
	int cmd = EDMA_IOCTL_CMD_MAKE(EDMA_CMD_COPY_2D);

	if (hEdma<0) return -1;
	param.chId = hndl;
	param.count = count;
	param.unData.copy2D = *prm;
	if (ioctl(hEdma, cmd, &param))
		return -1;

	return 0;
}

/*
// Edma_Map - Map Physical address to user land.
//
// RETURNS: 0 success, -1 if failed.
*/
char *Edma_Map( char *physAddr, int size )
{
	int fd;
	char *virtAddr;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) return NULL;
	virtAddr = mmap((void*)physAddr, size,
					PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,
					fd, (unsigned int)physAddr);
	close(fd);

	return virtAddr;
}

/*
// Edma_Unmap - Unmap address.
//
// RETURNS: 0 success, -1 if failed.
*/
int Edma_Unmap( char* virtAddr, int size )
{
	if (virtAddr)
		munmap((void*)virtAddr, size);    
    
	return 0;
}

/* End of file */

