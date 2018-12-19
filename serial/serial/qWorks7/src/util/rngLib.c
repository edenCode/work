#include <qWorks.h>
#include <rngLib.h>

/*
// rngInit - Init ring buffer;
//
// RETURNS: N/A
*/
int rngInit( RING* ringId, char* pBuf, int size )
{
	if (ringId==NULL || pBuf==NULL) return -1;
	ringId->head = 0;
	ringId->tail = 0;
	ringId->used = 0;
	ringId->size = size;
	ringId->pBuf = pBuf;
	return 0;
}

/*
// rngBufGet - get character from a ring buffer
//
// RETURNS: N/A
*/
int rngBufGet( RING* ringId, char *pCh )
{
	if (ringId==NULL || pCh==NULL) return -1;
	if (ringId->used==0) return -1;
	if (ringId->tail>=ringId->size)
		ringId->tail -= ringId->size;
	*pCh = ringId->pBuf[ringId->tail++];
	ringId->used --;
	return 0;
}

/*
// rngBufPut - put byte into a ring buffer
//
// RETURNS:
// The number of bytes actually put into the ring buffer;
// it may be less than number requested, even zero,
// if there is insufficient room in the ring buffer at the time of the call.
*/
int rngBufPut( RING* ringId, char ch )
{
	if (ringId==NULL) return -1;
	if (ringId->used==ringId->size) 
		return -1;
	if (ringId->head>=ringId->size)
		ringId->head -= ringId->size;
	ringId->pBuf[ringId->head++] = ch;
	ringId->used ++;
	return 0;
}

/*
// rngFlush - make a ring buffer empty
//
// RETURNS: N/A
*/
void rngFlush( RING* ringId )
{
	if (ringId==NULL) return;
    ringId->head = 0;
	ringId->tail = 0;
	ringId->used = 0;
}

/*
// rngIsEmpty - test if a ring buffer is empty
//
// RETURNS: TRUE if empty, FALSE if not.
*/
int rngIsEmpty ( RING* ringId )
{
	if (ringId==NULL) return FALSE;
    return (ringId->used==0);
}

/*
// rngIsFull - test if a ring buffer is full (no more room)
//
// RETURNS: TRUE if full, FALSE if not.
*/
int rngIsFull( RING* ringId )
{
	if (ringId==NULL) return FALSE;
    return (ringId->used==ringId->size);
}

/*
// rngFreeBytes - determines the number of free bytes in a ring buffer
//
// RETURNS: The number of unused bytes in the ring buffer.
*/
int rngFreeBytes( RING* ringId )
{
	if (ringId==NULL) return 0;
	return (ringId->size-ringId->used);
}

/*
// rngNBytes - determines the number of bytes in a ring buffer
//
// RETURNS: The number of bytes filled in the ring buffer.
*/
int rngNBytes( RING* ringId )
{
	if (ringId==NULL) return 0;
    return (ringId->used);
}

/* End of file */

