#ifndef	_RTC_H_
#define	_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>
#include <time.h>

/* function declarations */

int Rtc_Open( void );
int Rtc_Close( int hndl );
int Rtc_SetTime( int hndl, struct tm* tm );
int Rtc_GetTime( int hndl, struct tm* tm );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
