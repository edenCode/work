/* bLib.c - buffer manipulation library */

/*
DESCRIPTION
This library contains routines to manipulate buffers of variable-length
byte arrays.  Operations are performed on long words when possible, even
though the buffer lengths are specified in bytes.  This occurs only when
source and destination buffers start on addresses that are both odd or both
even.  If one buffer is even and the other is odd, operations must be done
one byte at a time, thereby slowing down the process.
*/

#define	ALIGNMENT 3

/*
// binvtBytes - invert the order of bytes in a buffer
//
// This routine inverts an entire buffer, byte by byte.  For example,
// the buffer {1, 2, 3, 4, 5} would become {5, 4, 3, 2, 1}.
*/
void binvtBytes( char *buf, int nbytes )
{
    char *buf_end = buf + nbytes - 1;
    char temp;

    while (buf < buf_end) 
	{
		temp = *buf;
		*buf = *buf_end;
		*buf_end = temp;

		buf_end--;
		buf++;
	}
}
	
/*
// bcopyBytes - copy one buffer to another one byte at a time
//
// This routine copies the first <nbytes> characters from <source> to
// <destination> one byte at a time.  This may be desirable if a buffer can
// only be accessed with byte instructions, as in certain byte-wide
// memory-mapped peripherals.
*/
void bcopyBytes( char *source, char *destination, int nbytes )
{
    char *dstend;
    int tmp = destination - source;

    if (tmp == 0) return;
    else if (tmp < 0 || tmp >= nbytes) 
	{
		/* forward copy */
		dstend = destination + nbytes;
		while (destination < dstend) 
		{
			*destination++ = *source++;
        }
	} 
	else 
	{
		/* backward copy */
		dstend       = destination;
		destination += nbytes - 1;
		source      += nbytes - 1;
		while (destination >= dstend) 
		{
            *destination-- = *source--;
        }
	}
}
	
/*
// bcopyWords - copy one buffer to another one word at a time
//
// This routine copies the first <nwords> words from <source> to <destination>
// one word at a time.  This may be desirable if a buffer can only be accessed
// with word instructions, as in certain word-wide memory-mapped peripherals.
// The source and destination must be word-aligned.
*/
void bcopyWords( char *source, char *destination, int nwords )
{
    short *dstend;
    short *src = (short*) source;
    short *dst = (short*) destination;
    int tmp = destination - source;   /* XXX does it work if MSB was 1 ? */
    int nbytes = nwords << 1;   /* convert to bytes */

    if (tmp == 0) return;
    else if (tmp < 0 || tmp >= nbytes) 
	{
		/* forward copy */
		dstend = dst + nwords;
		while (dst < dstend) *dst++ = *src++;
	} 
	else 
	{
		/* backward copy */
		dstend = dst;
        dst   += nwords - 1;
		src   += nwords - 1;
		while (dst >= dstend) *dst-- = *src--;
	}
}
	
/*
// bcopyLongs - copy one buffer to another one long word at a time
//
// This routine copies the first <nlongs> characters from <source> to
// <destination> one long word at a time.  This may be desirable if a buffer
// can only be accessed with long instructions, as in certain long-word-wide
// memory-mapped peripherals.  The source and destination must be
// long-aligned.
*/
void bcopyLongs( char *source, char *destination, int nlongs )
{
    long *dstend;
    long *src = (long *) source;
    long *dst = (long *) destination;
    int tmp = destination - source; /* XXX does it work if MSB was 1 ? */
    int nbytes = nlongs << 2; /* convert to bytes */

    if (tmp == 0) return;
    else if (tmp < 0 || tmp >= nbytes) 
	{ 
    	/* forward copy */
		dstend = dst + nlongs;
		while (dst < dstend) 
		{
			*dst++ = *src++;
        }
	} 
	else 
	{ 
    	/* backward copy */
        dstend = dst;
		dst   += nlongs - 1;
		src   += nlongs - 1;
		while (dst >= dstend) 
		{
			*dst-- = *src--;
		}
	}
}

/*
// bfillBytes - fill a buffer with a specified character
//
// This routine fills the first <nbytes> characters of a buffer with the
// character <ch>.  Filling is done in the most efficient way possible,
// which may be long-word, or even multiple-long-word stores, on some
// architectures.  In general, the fill will be significantly faster if
// the buffer is long-word aligned.  (For filling that is restricted to
// byte stores, see the manual entry for bfillBytes().)
*/
void bfillBytes( char *buf, int nbytes, int ch )
{
    long *pBuf;
    char *bufend = buf + nbytes;
    char *buftmp;
    long val;

    if (nbytes < 10) goto byte_fill;

    val = (ch << 24) | (ch << 16) | (ch << 8) | ch;

    /* start on necessary alignment */
    while ((int)buf & ALIGNMENT) 
	{
		*buf++ = ch;
    }
	
	/* last word boundary before bufend */
    buftmp = bufend - sizeof (long); 
    pBuf = (long *)buf;
    /* fill 4 bytes at a time; don't exceed buf endpoint */
    do 
	{ 
		*pBuf++ = val;	
	} while ((char*)pBuf < buftmp);
    buf = (char*)pBuf - sizeof(long);

    /* fill remaining bytes one at a time */
byte_fill:
    while (buf < bufend) 
	{
        *buf++ = ch;
    }
}
