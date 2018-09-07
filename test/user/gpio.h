#ifndef _GPIO_H
#define _GPIO_H

#include <user/qWorks.h>

/*
// GPIO Input line definition
*/
#define GPIO_IN_LINE0 0
#define GPIO_IN_LINE1 1
#define GPIO_IN_LINE2 2
#define GPIO_IN_LINE3 3

/*
// GPIO Trigger line definition
*/
#define GPIO_TRIG_LINE0 0
#define GPIO_TRIG_LINE1 1
#define GPIO_TRIG_LINE2 2
#define GPIO_TRIG_LINE3 3

/*
// GPIO output state
*/
#define GPIO_STATE_LO 0
#define GPIO_STATE_HI 1

/* Functions */

#ifdef __cplusplus
extern "C"
{
#endif

extern int Gpio_Open( void );
extern int Gpio_Close( int hndl );

extern int Gpio_GetInState( int hndl, int line );
extern int Gpio_SetRS485Dir( int hndl, int bIn );
extern int Gpio_SetLEDState( int hndl, int bOn );
extern int Gpio_SetTrigLine( int hndl, int line );
extern int Gpio_SetTrigLineWidth( int hndl, int width );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

