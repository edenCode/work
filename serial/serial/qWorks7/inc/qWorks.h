#ifndef _QWORKS_H_
#define _QWORKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Boolean defines. */
#if	!defined(BOOL)
# define BOOL int
#endif
#if	!defined(TRUE) || (TRUE!=1)
# define TRUE 1
#endif
#if	!defined(FALSE) || (FALSE!=0)
# define FALSE 0
#endif

#define NONE (-1)	/* for times when NULL won't do */
#define EOS '\0'	/* C string terminator */

/* Return status values */
#define OK 0
#define ERROR (-1)

/* Time-out defines */
#define NO_WAIT 0
#define WAIT_FOREVER (-1)

/* Common macros defines. */
#define MSB(x)	(((x) >> 8) & 0xff)	  	/* Most signif byte of 2-byte integer */
#define LSB(x)	((x) & 0xff)		  	/* Least signif byte of 2-byte integer*/
#define MSW(x) 	(((x) >> 16) & 0xffff) 	/* Most signif word of 2-word integer */
#define LSW(x) 	((x) & 0xffff) 		  	/* Least signif byte of 2-word integer*/

/* Swap the MSW with the LSW of a 32 bit integer. */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))

/* 32bit word byte/word swap macros. */
#define LLSB(x)		((x) & 0xff)
#define LNLSB(x) 	(((x) >> 8 ) & 0xff)
#define LNMSB(x) 	(((x) >> 16) & 0xff)
#define LMSB(x)	 	(((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) | (LNLSB(x) << 16) | (LNMSB(x) << 8) | (LMSB(x))) 

/* Byte offset of member in structure */
#define OFFSETOF(structure, member) ((int)&(((structure*)0)->member))

/* Size of a member of a structure */
#define MEMBER_SIZE(structure, member) (sizeof(((structure*)0)->member))

/* Number of elements in an array */
#define NELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

#ifndef MIN
# define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
# define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
# define ABS(a) (((a) < 0) ? (-a) : (a))
#endif

/* Round up/down macros. */
#define ROUND_UP(x, align)	 (((int)(x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align) ((int) (x) & ~(align - 1))
#define ALIGNED(x, align)	 (((int)(x) & (align - 1)) == 0)

/* POINT */
typedef struct qw_point
{
	int x; /* X Position */
	int y; /* Y Position */
} QW_POINT;

/* SIZE */
typedef struct qw_size
{
	int w; /* Width */
	int h; /* Height */
} QW_SIZE;

/* RECT */
typedef struct qw_rect
{
	int x; /* X Position */
	int y; /* Y Position */
	int w; /* Width */
	int h; /* Height */
} QW_RECT;

/*
// Range 
*/
typedef struct qw_range
{
	int min; /* Max value */
	int max; /* Min value */
} QW_RANGE;

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
