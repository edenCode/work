#ifndef	_CSL_GPIO_H_
#define	_CSL_GPIO_H_

#include <qWorks.h>

/*
// Ioctls
*/
#define CSL_GPIO_CMD_GET_VALUE    (0x00) /* Get GPIO value */
#define CSL_GPIO_CMD_SET_ETYPE    (0x01) /* Set GPIO Event type, 0: None, 1: rising, 2: falling, 3: both */

#define CSL_GPIO_CMD_RS485_DIR    (0x10) /* RS485 Direction */
#define CSL_GPIO_CMD_SET_LEDON    (0x11) /* LED control */

#define CSL_GPIO_CMD_TRIG_LINE    (0x20) /* Trigger Line, 0 - 3 */
#define CSL_GPIO_CMD_TRIG_PW      (0x21) /* Trigger Line Pulse Width in ms */

/*
// GPIO object. 
*/
typedef struct CSL_GPIO_OBJ
{
#ifdef __KERNEL__
	int pulseWidth; /* Trigger line PW in us */
#else
    int fd;
#endif
} CSL_GPIO_OBJ;

typedef struct CSL_GPIO_OBJ* CSL_GPIO_HANDLE;

/* functions declarations */

extern int Csl_GpioInit ( CSL_GPIO_HANDLE hndl );
extern int Csl_GpioExit ( CSL_GPIO_HANDLE hndl );
extern int Csl_GpioOpen ( CSL_GPIO_HANDLE hndl );
extern int Csl_GpioClose( CSL_GPIO_HANDLE hndl );
extern int Csl_GpioIoctl( CSL_GPIO_HANDLE hndl, int cmd, void *params );

#endif

/* End of file */
