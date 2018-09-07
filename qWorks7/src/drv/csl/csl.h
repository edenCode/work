#ifndef _CSL_H_
#define _CSL_H_

#include <qWorks.h>

/*
// CSL Version String 
*/
#define CSL_VERSION "1.00.00"

/*
// CSL Driver name 
*/
#define CSL_DRV_NAME "csl"

/*
// CSL Driver magic number 
*/
#define CSL_DRV_MAGICNUM     (0xCC)

/*
// CSL Ioctl cmd 
*/
#define CSL_IOCTL_CMD_MAKE(cmd)  ( _IO( CSL_DRV_MAGICNUM, cmd) )
#define CSL_IOCTL_CMD_GET(cmd)   ( _IOC_NR(cmd) )
#define CSL_IOCTL_CMD_VALID(cmd) ((_IOC_TYPE(cmd)==CSL_DRV_MAGICNUM ) ? 1 : 0)

/*
// CSL Module Ids 
*/
#define CSL_MODULE_GPIO      (0)
#define CSL_MODULE_FPGA      (1)
#define CSL_MODULE_MAX       (2)

#endif /* _CSL_H_ */

/* End of file */
