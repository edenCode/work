#ifndef	_CMEMK_H_
#define	_CMEMK_H_

#include <qWorks.h>
#include <linux/ioctl.h>

/*
// CMEM Version. 
*/
#define CMEM_VERSION "1.00.00"

/*
// CMEM Driver name and magic number. 
*/
#define CMEM_DRV_NAME  "cmem"
#define CMEM_DRV_MAGICNUM (0xDC)

#define CMEM_IOCTL_CMD_MAKE(cmd)     (_IO(CMEM_DRV_MAGICNUM, cmd))
#define CMEM_IOCTL_CMD_GET(cmd)      (_IOC_NR(cmd))
#define CMEM_IOCTL_CMD_IS_VALID(cmd) ((_IOC_TYPE(cmd) == CMEM_DRV_MAGICNUM ) ? 1 : 0)

/*
// CMEM flags. 
*/
#define CMEM_CACHED 1 /* Cached */

/*
// Ioctls
*/
#define CMEM_CMD_SETUP      (0x00) 
#define CMEM_CMD_ALLOC      (0x01) 
#define CMEM_CMD_FREE       (0x02)
#define CMEM_CMD_GETPHYS    (0x03)
#define CMEM_CMD_FREEPHYS   (0x04)
#define CMEM_CMD_CACHEWB    (0x05)
#define CMEM_CMD_CACHEINV   (0x06)
#define CMEM_CMD_CACHEWBINV (0x07)

/* CMEM block definition */
typedef struct CMEM_BLOCK 
{
    unsigned long addr; /* Address */
    int size; /* Memory size in bytes */
    int flag; /* CMEM flag */
    int alignment; /* Only used for allocations, must be power of 2 */
} CMEM_BLOCK;

#define CMEM_NUM_REGION 2 /* CMEM Region */

/*
// CMEM Region. 
*/
typedef struct CMEM_REGION
{
	/* Object specified */
	unsigned long phys; /* Physical address */
	unsigned long virt; /* Kernel virtual address */
    int           size; /* Memory size in bytes */
	int           flag; /* Memory flag */
#ifdef __KERNEL__
    PARTION    memPart; /* Memory Partition */
    struct mutex mutex; /* Access mutex */
#endif
} CMEM_REGION;

/*
// CMEM object. 
*/
typedef struct CMEM_OBJ
{
	CMEM_REGION regions[CMEM_NUM_REGION]; /* Regions */
} CMEM_OBJ;

#endif

/* End of file */
