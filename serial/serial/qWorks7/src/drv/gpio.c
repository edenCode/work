#include <gpio.h>
#include <drvLib.h>
#include "csl/csl.h"
#include "csl/gpio.h"

/*
// Gpio_Open - Create GPIO Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_Open( void )
{
	return Drv_Open(CSL_MODULE_GPIO);
}

/*
// Gpio_Close - Delete GPIO Object.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_Close( int hndl )
{
	return Drv_Close(hndl);
}

/*
// Gpio_GetInState - Get GPIO state.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_GetInState( int hndl, int line )
{
	int data = 0;

	if (line<GPIO_IN_LINE0 || line>GPIO_IN_LINE3)
	{
		return -1;
	}
	if (Drv_Ioctl(hndl, CSL_GPIO_CMD_GET_VALUE, &data))
	{
		return -1;
	}
	return data;
}

/*
// Gpio_SetRS485Dir - Set RS485 DIR.
//
// RETURNS: 0 sucess, -1 if failed.
*/
int Gpio_SetRS485Dir( int hndl, int bIn )
{
	if (Drv_Ioctl(hndl, CSL_GPIO_CMD_RS485_DIR, &bIn))
	{
		return -1;
	}
	return 0;
}

/*
// Gpio_SetLEDState - Set LED state.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_SetLEDState( int hndl, int bOn )
{
	return Drv_Ioctl(hndl, CSL_GPIO_CMD_SET_LEDON, &bOn);
}

/*
// Gpio_SetTrigLine - Set trigger line.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_SetTrigLine( int hndl, int line )
{
	if (line<GPIO_TRIG_LINE0 || line>GPIO_TRIG_LINE3)
	{
		return -1;
	}
	if (Drv_Ioctl(hndl, CSL_GPIO_CMD_TRIG_LINE, &line))
	{
		return -1;
	}
	
	return 0;
}

/*
// Gpio_SetTrigLineWidth - Set trigger line pulse width.
//
// RETURNS: 0 success, -1 if failed.
*/
int Gpio_SetTrigLineWidth( int hndl, int width )
{
	return Drv_Ioctl(hndl, CSL_GPIO_CMD_TRIG_PW, &width);
}

/* End of file */

