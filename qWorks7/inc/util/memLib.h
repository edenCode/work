#ifndef _MEMLIB_H_
#define _MEMLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>
#include <dllLib.h>

/* macros for memory align, architecture specified */
#ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE	  4
#endif

#define MEM_ROUND_UP(x)	  ROUND_UP(x, MEM_ALIGN_SIZE)
#define MEM_ROUND_DOWN(x) ROUND_DOWN(x, MEM_ALIGN_SIZE)
#define MEM_ALIGNED(x)	  ALIGNED(x, MEM_ALIGN_SIZE)

/* macros for getting to next and previous blocks */
#define NEXT_HDR(pHdr)  ((BLOCK_HDR*)((char*)(pHdr) + (2 * (pHdr)->nWords)))
#define PREV_HDR(pHdr)	((pHdr)->pPrevHdr)

/* macros for converting between the "block" that caller knows
 * (actual available data area) and the block header in front of it */
#define HDR_TO_BLOCK(pHdr)	 ((char*)((int) pHdr + sizeof (BLOCK_HDR)))
#define BLOCK_TO_HDR(pBlock) ((BLOCK_HDR*)((int)pBlock - sizeof(BLOCK_HDR)))

/* macros for converting between the "node" that is strung on the freelist
 * and the block header in front of it */
#define HDR_TO_NODE(pHdr)	(&((FREE_BLOCK*)pHdr)->node)
#define NODE_TO_HDR(pNode)	((BLOCK_HDR*)((int) pNode - OFFSETOF(FREE_BLOCK, node)))

/* typedefs */
typedef struct partion 
{
    DL_LIST       freeList;	/* list of free blocks */
    unsigned    totalWords;	/* total number of words in pool */
    unsigned minBlockWords;	/* min blk size in words includes hdr */

    /* allocation statistics */
    unsigned curBlocksAllocated; /* current # of blocks allocated */
    unsigned  curWordsAllocated; /* current # of words allocated */
    unsigned cumBlocksAllocated; /* cumulative # of blocks allocated */
    unsigned  cumWordsAllocated; /* cumulative # of words allocated */
} PARTION;

/* Partition statistics structure */
typedef struct parttion_stat 
{
    unsigned numBytesFree, /* Number of Free Bytes in Partition       */
		    numBlocksFree, /* Number of Free Blocks in Partition      */
	     maxBlockSizeFree, /* Maximum block size that is free.	      */
		    numBytesAlloc, /* Number of Allocated Bytes in Partition  */
	       numBlocksAlloc; /* Number of Allocated Blocks in Partition */
} PARTION_STAT;
    
/* Block header. */
typedef struct blockhdr /* BLOCK_HDR */ 
{
    struct blockhdr* pPrevHdr; /* pointer to previous block hdr */
    unsigned      nWords : 31; /* size in words of this block */
    unsigned        free :  1; /* TRUE = this block is free */
} BLOCK_HDR;

/* 
 * Free block header. 
 * NOTE: The size of this data structure is already aligned on 
 * for 4, 8, and 16 bytes alignments.
 */
typedef struct freeblock 
{
    BLOCK_HDR hdr;
    DL_NODE  node; /* freelist links */
} FREE_BLOCK;

/* function declarations */

void Mem_Init( PARTION* pMemPart, char *pPool, unsigned poolSize );
int Mem_Add( PARTION* pMemPart, char *pPool, unsigned poolSize );
void* Mem_Alloc( PARTION* pMemPart, unsigned nBytes );
void* Mem_AllocEx( PARTION* pMemPart, unsigned nBytes, unsigned alignment );
void* Mem_Realloc( PARTION* pMemPart, char *pBlock, unsigned nBytes );
int Mem_Free( PARTION* pMemPart, char *pBlock );
int Mem_FindMax( PARTION* pMemPart );
int Mem_Validate( PARTION* pMemPart, BLOCK_HDR *pHdr, int isFree );
int Mem_Info( PARTION* pMemPart, PARTION_STAT *pPartStats );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
