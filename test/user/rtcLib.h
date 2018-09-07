#ifndef	_RTC_LIB_H_
#define	_RTC_LIB_H_

#include <ufx.h>
#include <time.h>

/* function declarations */

extern int Rtc_Open( void );
extern int Rtc_Close( int hndl );
extern int Rtc_SetTime( int hndl, struct tm* tm );
extern int Rtc_GetTime( int hndl, struct tm* tm );

#endif

/* End of file */
