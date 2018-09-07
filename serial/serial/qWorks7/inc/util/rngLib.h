#ifndef _RNG_LIB_H_
#define _RNG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 
// Ring buffer definition 
*/
typedef struct ring 
{
    int   head; /* head pointer */
    int   tail; /* tail pointer */
	int   used; /* Used count */
    int   size; /* size of ring in bytes */
    char* pBuf; /* pointer to start of buffer */
} RING, *RNG_ID;

/* function declarations */

int rngInit( RING* ringId, char* pBuf, int size );
int rngBufGet( RING* ringId, char *pCh );
int rngBufPut( RING* ringId, char  chr );
void rngFlush( RING* ringId );
int rngIsFull( RING* ringId );
int rngIsEmpty( RING* ringId );
int rngNBytes( RING* ringId );
int rngFreeBytes( RING* ringId );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
