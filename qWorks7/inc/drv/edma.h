#ifndef _EDMA_H_
#define _EDMA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* EDMA Copy 1D */
typedef struct edma_copy1d 
{
	unsigned long srcPhysAddr;
	unsigned long dstPhysAddr;
	int size;  // in bytes
} EDMA_COPY1D;

/* EDMA Copy 2D */
typedef struct edma_copy2d 
{
	unsigned long srcPhysAddr;
	unsigned long dstPhysAddr;
	int copyWidth;   // in bytes
	int copyHeight;  // in lines
	int srcOffsetH;  // in bytes
	int dstOffsetH;  // in bytes
	int skipH; // Horizontal Skip, 0: No skip, >= 1 copy 'skipH' bytes, skip 'skipH' bytes. Eg to skip alternate bytes set skipH to 1  
} EDMA_COPY2D;

/* EDMA Fill 1D */
typedef struct edma_fill1d 
{
	unsigned long dstPhysAddr;
	unsigned long dstVirtAddr;
	int fillValue;  // 32-bit fill value
	int size; // in bytes, must be multiple of 4 bytes, must be >= 32 bytes
} EDMA_FILL1D;

/* EDMA Fill 2D */
typedef struct edma_fill2d 
{
	unsigned long dstPhysAddr;
	unsigned long dstVirtAddr;
	int fillValue;  // 32-bit fill value
	int copyWidth;  // in bytes, must be multiple of 4 bytes, must be >= 16 bytes
	int copyHeight; // in lines
	int dstOffsetH; // in bytes, must be multiple of 16 bytes
} EDMA_FILL2D;

/* function declarations */

int Edma_Init( void );
int Edma_Cleanup( void );

int Edma_Open( int dmaMode, int maxTransfers );
int Edma_Close( int hndl );

int Edma_Copy1D( int hndl, EDMA_COPY1D *prm, int count );
int Edma_Copy2D( int hndl, EDMA_COPY2D *prm, int count );
int Edma_Fill1D( int hndl, EDMA_FILL1D *prm, int count );
int Edma_Fill2D( int hndl, EDMA_FILL2D *prm, int count );

char *Edma_Map( char* physAddr, int size );
int   Edma_Unmap( char* virtAddr, int size );

#ifdef __cplusplus
}
#endif

#endif  /* _EDMA_H_ */

