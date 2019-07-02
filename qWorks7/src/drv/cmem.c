#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "csl/cmemk.h"

/*
// Locals
*/
static int hCmem = -1;

/*
// Cmem_Init - Init CMEM Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Cmem_Init( unsigned long base, int size, int flag )
{
	char name[32];
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_SETUP);

	if (hCmem==-1)
	{
		sprintf(name, "/dev/%s", CMEM_DRV_NAME);
		hCmem = open(name, O_RDWR);
		if (hCmem<0) return -1;
		
		cblk.addr = base;
		cblk.size = size;
		cblk.flag = flag;
		cblk.alignment = 0;
		if (ioctl(hCmem, cmd, &cblk))
		{
			close(hCmem);
			hCmem = -1;
			return -1;
		}
	}
	
	return 0;
}

/*
// Cmem_Cleanup - Cleanup CMEM Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Cmem_Cleanup( void )
{
	if (hCmem==-1)
	{
		return -1;
	}
	close(hCmem);
	hCmem = -1;

	return 0;
}

/*
// Cmem_Alloc - Allocate memory from CMEM.
//
// RETURNS: 0 success, -1 if failed.
*/
uint8_t* Cmem_Alloc( int nBytes, int align, int flag )
{
	void *virtAddr;
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_ALLOC);

	cblk.addr = 0;
	cblk.size = nBytes;
	cblk.flag = flag;
	cblk.alignment = align;
	if (ioctl(hCmem, cmd, &cblk))
		return NULL;
	
	/* Map to user-land address space */
	virtAddr = mmap(0, cblk.size,
					PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED,
					hCmem, cblk.addr);
	if (virtAddr==NULL) 
	{
		cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_FREEPHYS);
		ioctl(hCmem, cmd, &cblk);
		return NULL;
	}
	return (uint8_t*)virtAddr;
}

/*
// Cmem_Free - Free memory to CMEM.
//
// RETURNS: 0 success, -1 if failed.
*/
int Cmem_Free( uint8_t* addr, int size )
{
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_FREE);

	cblk.addr = (unsigned long)addr;
	cblk.size = size;
	cblk.flag = 0;
	cblk.alignment = 0;
	if (ioctl(hCmem, cmd, &cblk))
		return -1;
	/* Un-map user-land address */
	if (munmap(addr, size) == -1) 
        return -1;
	return 0;
}

/*
// Cmem_GetPhys - Get Physical address.
//
// RETURNS: 0 success, -1 if failed.
*/
unsigned long Cmem_GetPhys( uint8_t* addr )
{
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_GETPHYS);

	cblk.addr = (unsigned long)addr;
	cblk.size = 0;
	cblk.flag = 0;
	cblk.alignment = 0;
	if (ioctl(hCmem, cmd, &cblk))
		return 0;
	return cblk.addr;
}

/*
// Cmem_CacheWb - Do CMEM Cache write-back.
//
// RETURNS: 0 success, -1 if failed.
*/
void Cmem_CacheWb( uint8_t* addr, int size )
{
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_CACHEWB);

	cblk.addr = (unsigned long)addr;
	cblk.size = size;
	cblk.flag = 0;
	cblk.alignment = 0;
	ioctl(hCmem, cmd, &cblk);
}

/*
// Cmem_CacheInv - Do CMEM Cache invalidate.
//
// RETURNS: 0 success, -1 if failed.
*/
void Cmem_CacheInv( uint8_t* addr, int size )
{
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_CACHEINV);

	cblk.addr = (unsigned long)addr;
	cblk.size = size;
	cblk.flag = 0;
	cblk.alignment = 0;
	ioctl(hCmem, cmd, &cblk);
}

/*
// Cmem_CacheWbInv - Do CMEM Cache Write-back and invalidate.
//
// RETURNS: 0 success, -1 if failed.
*/
void Cmem_CacheWbInv( uint8_t* addr, int size )
{
	CMEM_BLOCK cblk;
	int cmd = CMEM_IOCTL_CMD_MAKE(CMEM_CMD_CACHEWBINV);

	cblk.addr = (unsigned long)addr;
	cblk.size = size;
	cblk.flag = 0;
	cblk.alignment = 0;
	ioctl(hCmem, cmd, &cblk);
}

/* End of file */

