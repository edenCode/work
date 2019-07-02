#ifndef _WDT_H_
#define _WDT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

int Wdt_Open( int tm );
int Wdt_Close( int hndl );
int Wdt_Enable( int hndl );
int Wdt_SetTime( int hndl, int tm );
int Wdt_KeepAlive( int hndl );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
