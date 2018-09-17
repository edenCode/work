#ifndef	_NCSC_H_
#define	_NCSC_H_

#include <linux/ioctl.h>

/* NCSC Version. */
#define NCSC_VERSION "1.00.00"

/* NCSC Driver name and magic number. */
#define NCSC_DRV_NAME  "ncsc"
#define NCSC_DRV_MAGICNUM (0xDC)

#define NCSC_IOCTL_CMD_MAKE(cmd)     (_IO(NCSC_DRV_MAGICNUM, cmd))
#define NCSC_IOCTL_CMD_GET(cmd)      (_IOC_NR(cmd))
#define NCSC_IOCTL_CMD_IS_VALID(cmd) ((_IOC_TYPE(cmd) == NCSC_DRV_MAGICNUM ) ? 1 : 0)

/* Ioctls */
#define NCSC_CMD_ENABLE (0x00) /* H-Active */

#endif

/* End of file */
