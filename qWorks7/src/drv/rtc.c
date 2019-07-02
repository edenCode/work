#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/ioctl.h>
#include <linux/rtc.h>
#include <osError.h>
#include <rtc.h>

/*
// RTC device
*/
#define RTC_DEV_NAME "/dev/rtc0"

/*
// Rtc_Open - Create RTC Object. 
//  
// RETURNS: 0: OK; -1 : NG.
*/ 
int Rtc_Open( void )
{
	return open(RTC_DEV_NAME, O_RDWR);
}

/*
// Rtc_Close - Delete RTC Object.
//
// RETURNS: 0: OK; -1 : NG.
*/
int Rtc_Close( int hndl )
{
	return close(hndl);
}

/*
// Rtc_SetTime - Set RTC time.
//
// RETURNS: 0 - Success, -1 - Failed.
*/
int Rtc_SetTime( int hndl, struct tm* tm )
{
	return ioctl(hndl, RTC_SET_TIME, (struct rtc_time*)tm);
}

/*
// Rtc_GetTime - Get RTC time.
//
// RETURNS: 0 - Success, -1 - Failed.
*/
int Rtc_GetTime( int hndl, struct tm* tm )
{
	return ioctl(hndl, RTC_RD_TIME, (struct rtc_time*)tm);
}

/* End of file */

