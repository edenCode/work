#include <osError.h>

/*
// Error code
*/
static int OSErrorNo = 0;

/* 
// OSError_Get - Get last error code. 
//
// RETURNS: Last error code.
*/
int OSError_Get( void )
{
	return OSErrorNo;
}

/* 
// OSError_Set - Set last error code. 
//
// RETURNS: N/A.
*/
void OSError_Set( int err )
{
	OSErrorNo = err;
}

/* End of file */

