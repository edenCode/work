#ifndef _EDMAK_H_
#define _EDMAK_H_

#include <linux/ioctl.h>
#include <edma.h>

/*
// EDMA Version. 
*/
#define EDMA_VERSION "1.00.00"

#define EDMA_NON_CACHE_ALLOC_BASE_PHYS_ADDR  (0x90000000)

#define EDMA_DEV_MAX_CH  (64)
#define EDMA_MODE_NORMAL 0 /* normal memcpy or memfill type DMA's */
#define EDMA_MAX_NORMAL_TRANSFERS (16)

/*
// EDMA Driver name and magic number. 
*/
#define EDMA_DRV_NAME           "edma"
#define EDMA_DRV_MAGICNUM       (0xDA)

#define EDMA_CMD_CH_OPEN        (0x01)
#define EDMA_CMD_CH_CLOSE       (0x02)
#define EDMA_CMD_COPY_1D        (0x03)
#define EDMA_CMD_COPY_2D        (0x04)
#define EDMA_CMD_FILL_1D        (0x05)
#define EDMA_CMD_FILL_2D        (0x06)

#define EDMA_IOCTL_CMD_MAKE(cmd)     (_IO( EDMA_DRV_MAGICNUM, cmd))
#define EDMA_IOCTL_CMD_GET(cmd)      (_IOC_NR(cmd))
#define EDMA_IOCTL_CMD_IS_VALID(cmd) ((_IOC_TYPE(cmd) == EDMA_DRV_MAGICNUM ) ? 1 : 0)


/* EDMA Copy/fill Parameters */
typedef struct edma_copyfill 
{
	int chId; /* Channel ID */
	int count; /* Transfer count */
	union 
	{
		EDMA_COPY1D copy1D;
		EDMA_COPY2D copy2D;
		EDMA_FILL1D fill1D;
		EDMA_FILL2D fill2D;
	} unData;
} EDMA_COPYFILL;

/* EDMA channel */
typedef struct EDMA_CHANNEL
{
	int chId; /* Channel ID */
	int mode; /* Mode */
	int maxTransfers; /* Max transfers */
} EDMA_CHANNEL;

#endif  /* _EDMA_H_ */

