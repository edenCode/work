#include <bLib.h>
#include <memLib.h>

/* forward static functions */
static BLOCK_HDR*
memAlignedBlockSplit(PARTION* pMemPart, BLOCK_HDR *pHdr, unsigned nWords, unsigned minWords, unsigned alignment);

/*
// Mem_Init - initialize a memory partition
//
// This routine initializes a partition free list, seeding it with the
// memory block passed as an argument.  It must be called exactly once
// for each memory partition created.
*/
void Mem_Init( PARTION* pMemPart, char *pPool, unsigned poolSize )
{
    /* initialize partition descriptor */
    bfillBytes ((char*) pMemPart, sizeof (*pMemPart), 0);
    pMemPart->minBlockWords = sizeof (FREE_BLOCK) >> 1;
    DLL_INIT (&pMemPart->freeList);	/* init. free list */
    (void) Mem_Add (pMemPart, pPool, poolSize);
}

/*
// Mem_Add - add memory to a memory partition
//
// This routine adds memory to a specified memory partition already created
// The memory added need not be contiguous with memory previously assigned 
// to the partition.
//
// RETURNS: OK or ERROR.
*/
int Mem_Add( PARTION* pMemPart, char *pPool, unsigned poolSize )
{
    BLOCK_HDR *pHdrStart;
    BLOCK_HDR *pHdrMid;
    BLOCK_HDR *pHdrEnd;
    char *tmp;
    int reducePool; /* reduce size of pool by this amount*/

    /* insure that the pool starts on an even byte boundary */
    tmp  = (char*) MEM_ROUND_UP (pPool);/* get actual start */
    reducePool = tmp - pPool;
    if (poolSize >= reducePool)		 	/* adjust length */
        poolSize -= reducePool;
    else
        poolSize = 0;

    pPool  = tmp;

    /*
     * Make sure poolSize is an even multiple of the memory alignment size
     * and is large enough to include at least one valid free block and
     * three header blocks.
     */
    poolSize = MEM_ROUND_DOWN (poolSize);
    if (poolSize < ((sizeof (BLOCK_HDR) * 3) + (pMemPart->minBlockWords * 2)))
        return (-1);

    /* initialize three blocks -
     * one at each end of the pool for end cases and real initial free block */
    pHdrStart		    = (BLOCK_HDR *) pPool;
    pHdrStart->pPrevHdr = NULL;
    pHdrStart->free     = FALSE;
    pHdrStart->nWords   = sizeof (BLOCK_HDR) >> 1;

    pHdrMid		        = NEXT_HDR (pHdrStart);
    pHdrMid->pPrevHdr   = pHdrStart;
    pHdrMid->free       = TRUE;
    pHdrMid->nWords     = (poolSize - 2 * sizeof (BLOCK_HDR)) >> 1;

    pHdrEnd		        = NEXT_HDR (pHdrMid);
    pHdrEnd->pPrevHdr   = pHdrMid;
    pHdrEnd->free       = FALSE;
    pHdrEnd->nWords     = sizeof (BLOCK_HDR) >> 1;

    dllInsert (&pMemPart->freeList, (DL_NODE*) NULL, HDR_TO_NODE (pHdrMid));
    pMemPart->totalWords  += (poolSize >> 1);

    return (0);
}

/*
// Mem_AllocEx - allocate aligned memory from a partition
//
// This routine allocates a buffer of size <nBytes> from a specified
// partition.  Additionally, it insures that the allocated buffer begins on a
// memory address evenly divisible by <alignment>.  The <alignment> parameter
// must be a power of 2.
//
// RETURNS:
// A pointer to the newly allocated block, or NULL if the buffer could not be
// allocated.
*/
void* Mem_AllocEx( PARTION* pMemPart, unsigned nBytes, unsigned alignment )
{
    unsigned nWords;
    unsigned nWordsExtra;
    DL_NODE   *pNode;
    BLOCK_HDR *pHdr;
    BLOCK_HDR *pNewHdr;
    BLOCK_HDR origpHdr;

    /* get actual size to allocate; add overhead, check for minimum */
    nWords = (MEM_ROUND_UP (nBytes) + sizeof (BLOCK_HDR)) >> 1;

    /* check if we have an overflow. if so, set errno and return NULL */
    if ((nWords << 1) < nBytes) return (NULL);

    if (nWords < pMemPart->minBlockWords)
		nWords = pMemPart->minBlockWords;

    /* first fit */
    pNode = DLL_FIRST (&pMemPart->freeList);

    /* We need a free block with extra room to do the alignment.  
     * Worst case we'll need alignment extra bytes.
     */
    nWordsExtra = nWords + alignment / 2;
    while(1) 
	{
		while (pNode != NULL) 
		{
	   		if ((NODE_TO_HDR (pNode)->nWords > nWordsExtra) ||
				((NODE_TO_HDR (pNode)->nWords == nWords) &&
				(ALIGNED (HDR_TO_BLOCK(NODE_TO_HDR(pNode)), alignment))))
				break;
				pNode = DLL_NEXT (pNode);
		}
	
		if (pNode == NULL) return (NULL);

		pHdr = NODE_TO_HDR (pNode);
		origpHdr = *pHdr;

	   /* now we split off from this block, the amount required by the user;
		* note that the piece we are giving the user is at the end of the
		* block so that the remaining piece is still the piece that is
		* linked in the free list;  if memAlignedBlockSplit returned NULL,
		* it couldn't split the block because the split would leave the
		* first block too small to be hung on the free list, so we continue
		* trying blocks.
		*/
		pNewHdr = memAlignedBlockSplit (pMemPart, pHdr, nWords, 
			pMemPart->minBlockWords, alignment);
		if (pNewHdr != NULL) 
		{
			pHdr = pNewHdr;	/* give split off block */
			break;
		}
		pNode = DLL_NEXT (pNode);
	}

    /* mark the block we're giving as not free */
    pHdr->free = FALSE;

    /* update allocation statistics */
    pMemPart->curBlocksAllocated ++;
    pMemPart->cumBlocksAllocated ++;
    pMemPart->curWordsAllocated  += pHdr->nWords;
    pMemPart->cumWordsAllocated  += pHdr->nWords;

    return ((void *) HDR_TO_BLOCK (pHdr));
}

/*
// Mem_Alloc - allocate a block of memory from a partition
//
// This routine allocates a block of memory from a specified partition. 
// The size of the block will be equal to or greater than <nBytes>.  
// The partition must already be created with memCreate().
//
// RETURNS:
// A pointer to a block, or NULL if the call fails.
*/
void* Mem_Alloc( PARTION* pMemPart, unsigned nBytes )
{
    /* partition is local */
    return (Mem_AllocEx (pMemPart, nBytes, MEM_ALIGN_SIZE));
}

/*
// memBlockSplit - split a block into two blocks
//
// This routine splits the block pointed to into two blocks.  The second
// block will have nWords words in it.  A pointer is returned to this block.
// If either resultant block would end up having less than minWords in it,
// NULL is returned.
//
// RETURNS: A pointer to the second block, or NULL.
*/
static BLOCK_HDR* memBlockSplit( BLOCK_HDR *pHdr, unsigned nWords, unsigned minWords )
{
    unsigned wordsLeft;
    BLOCK_HDR *pNewHdr;

    /* check if block can be split */
    if ((nWords < minWords) ||
        ((wordsLeft = (pHdr->nWords - nWords)) < minWords))
        return (NULL); /* not enough space left */

    /* adjust original block size and create new block */
    pHdr->nWords      = wordsLeft;
    pNewHdr           = NEXT_HDR (pHdr);
    pNewHdr->pPrevHdr = pHdr;
    pNewHdr->nWords   = nWords;
    pNewHdr->free     = pHdr->free;

    /* fix next block */
    NEXT_HDR (pNewHdr)->pPrevHdr = pNewHdr;

    return (pNewHdr);
}

/*
// Mem_Realloc - reallocate a block of memory in a specified partition
//
// This routine changes the size of a specified block of memory and returns a
// pointer to the new block.  The contents that fit inside the new size (or
// old size if smaller) remain unchanged.  The memory alignment of the new
// block is not guaranteed to be the same as the original block.
//
// If <pBlock> is NULL, this call is equivalent to Mem_Alloc().
//
// RETURNS:
// A pointer to the new block of memory, or NULL if the call fails.
*/
void* Mem_Realloc( PARTION* pMemPart, char *pBlock, unsigned nBytes )
{
    BLOCK_HDR *pHdr = BLOCK_TO_HDR (pBlock);
    BLOCK_HDR *pNextHdr;
    unsigned nWords;
    void *pNewBlock;

    /* Call Mem_Alloc() if user passed us a NULL pointer */
    if (pBlock == NULL)
        return (Mem_Alloc (pMemPart, (unsigned) nBytes));

    /* get actual new size; round-up, add overhead, check for minimum */
    nWords = (MEM_ROUND_UP(nBytes) + sizeof (BLOCK_HDR)) >> 1;
    if (nWords < pMemPart->minBlockWords)
		nWords = pMemPart->minBlockWords;

    /* test if we are trying to increase size of block */
    if (nWords > pHdr->nWords) 
	{
		pNextHdr = NEXT_HDR (pHdr);
		if (!pNextHdr->free || ((pHdr->nWords + pNextHdr->nWords) < nWords)) 
		{
			if ((pNewBlock = Mem_Alloc (pMemPart, nBytes)) == NULL)
				return (NULL);

			bcopyBytes (pBlock, (char *) pNewBlock,
				(int) (2 * pHdr->nWords - sizeof (BLOCK_HDR)));
			(void) Mem_Free (pMemPart, pBlock);

			return (pNewBlock);		/* RETURN, don't fall through */
		}
		else 
		{
			dllRemove (&pMemPart->freeList, HDR_TO_NODE (pNextHdr));
			pHdr->nWords += pNextHdr->nWords;	/* fix size */
			pMemPart->curWordsAllocated += pNextHdr->nWords;/* fix stats */
			pMemPart->cumWordsAllocated += pNextHdr->nWords;

			NEXT_HDR (pHdr)->pPrevHdr = pHdr; /* fix next */
		}
	}

    /* split off any extra and give it back;
     * note that this covers both the case of a realloc for smaller space
     * and the case of a realloc for bigger space that caused a coalesce
     * with the next block that resulted in larger than required block */
    pNextHdr = memBlockSplit (pHdr, pHdr->nWords - nWords, pMemPart->minBlockWords);
    if (pNextHdr != NULL) 
	{
		(void) Mem_Free (pMemPart, HDR_TO_BLOCK (pNextHdr));
		pMemPart->curBlocksAllocated++;
	}

    return ((void *) pBlock);
}

/*
// Mem_Free - free a block of memory in a partition
//
// This routine returns to a partition's free memory list a block of 
// memory previously allocated with Mem_Alloc().
//
// RETURNS: OK, or ERROR if the block is invalid.
*/
int Mem_Free( PARTION* pMemPart, char *pBlock )
{
    BLOCK_HDR *pHdr;
    unsigned   nWords;
    BLOCK_HDR *pNextHdr;

    if (pBlock == NULL) return (0);

    pHdr   = BLOCK_TO_HDR (pBlock);
    nWords = pHdr->nWords; 

    /* check if we can coalesce with previous block;
     * if so, then we just extend the previous block,
     * otherwise we have to add this as a new free block */
    if (PREV_HDR (pHdr)->free) 
	{
		pHdr->free = FALSE;	    /* this isn't a free block */
		pHdr = PREV_HDR (pHdr);	/* coalesce with prev block */
		pHdr->nWords += nWords;
	}
    else 
	{
		pHdr->free = TRUE;	/* add new free block */
		dllInsert (&pMemPart->freeList, (DL_NODE *) NULL, HDR_TO_NODE (pHdr));
	}

    /* check if we can coalesce with next block;
     * if so, then we can extend our block delete next block from free list */
    pNextHdr = NEXT_HDR (pHdr);
    if (pNextHdr->free)	
	{
		pHdr->nWords += pNextHdr->nWords;	/* coalesce with next */
		dllRemove (&pMemPart->freeList, HDR_TO_NODE (pNextHdr));
	}

    /* fix up prev info of whatever block is now next */
    NEXT_HDR (pHdr)->pPrevHdr = pHdr;

    /* adjust allocation stats */
    pMemPart->curBlocksAllocated--;
    pMemPart->curWordsAllocated -= nWords;

    return (0);
}

/*
// Mem_Validate - check validity of block
//
// This routine checks the validity of a block.
*/
int Mem_Validate( PARTION* pMemPart, BLOCK_HDR *pHdr, int isFree )
{
    int valid;

    valid = MEM_ALIGNED (pHdr)						/* aligned */
            && MEM_ALIGNED (2 * pHdr->nWords)		/* size is round */
            && (pHdr->nWords <= pMemPart->totalWords)	/* size <= total */
			&& (pHdr->free == isFree)				/* right alloc-ness */
			&& (pHdr == PREV_HDR (NEXT_HDR (pHdr)))	/* matches next block */
			&& (pHdr == NEXT_HDR (PREV_HDR (pHdr)));/* matches prev block */

    return (valid);
}

/*
// memAlignedBlockSplit - split a block on the free list into two blocks
//
// This routine is like memBlockSplit, but also aligns the data block to the
// given alignment.  The block looks like this after it is split:
//
//        |----------------------------------------------------------------|
//        ^                      ^                       ^                 ^
//        |                      |                       | space left over |
//        |                      |<------- nWords ------>| after alignment |
//        |                      |                       |                 |
//  block begin       new aligned buffer begin         end           block end
//
//
// After the split, if the first block has less than minWords in it, the
// block is rejected, and null is returned, since we can't place this first
// block on the free list.
// If the space succeeding the newly allocated aligned buffer is less than the
// minimum block size, then the bytes are added to the newly allocated buffer.
// If the space is greater than the minimum block size,  then a new memory
// fragment is created and added to the free list.
// Care must be taken to insure that the orignal block passed in
// to be split has at least (nWords + alignment/2) words in it.  
// If the block has exactly nWords and is already aligned to the given alignment,
// it will be deleted from the free list and returned unsplit.
// Otherwise, the second block will be returned.
//
// RETURNS: A pointer to a BLOCK_HDR 
*/
static BLOCK_HDR* memAlignedBlockSplit( PARTION* pMemPart, BLOCK_HDR *pHdr, unsigned nWords, 
                      unsigned minWords, unsigned alignment)
{
    BLOCK_HDR *pNewHdr;
    BLOCK_HDR *pNextHdr;
    char *endOfBlock;
    char *pNewBlock;
    int blockSize;

    /* calculate end of pHdr block */
    endOfBlock = (char *) pHdr + (pHdr->nWords * 2); 

    /* caluclate unaligned beginning of new block */ 
    pNewBlock = (char *) ((unsigned) endOfBlock - 
		((nWords - sizeof (BLOCK_HDR) / 2) * 2));

    /* align the beginning of the block */
    pNewBlock = (char *)((unsigned) pNewBlock & ~(alignment - 1));
    pNewHdr   = BLOCK_TO_HDR (pNewBlock);

    /* adjust original block's word count */
    blockSize = ((char *) pNewHdr - (char *) pHdr) / 2;
    if (blockSize < minWords) 
	{
		if (pNewHdr == pHdr)
			dllRemove (&pMemPart->freeList, HDR_TO_NODE (pHdr));
		else
			return (NULL);
	}
    else 
	{
		pNewHdr->pPrevHdr = pHdr;
		pHdr->nWords = blockSize;
	}

    /* check to see if space left over after we aligned the new buffer
     * is big enough to be a fragment on the free list.
     */
    if (((unsigned) endOfBlock - (unsigned) pNewHdr - 
		(nWords * 2)) < (minWords * 2)) 
	{
		/* nope - give all the memory to the newly allocated block */
		pNewHdr->nWords = (endOfBlock - pNewBlock + sizeof (BLOCK_HDR)) / 2;
		pNewHdr->free   = TRUE; 

		/* fix next block to point to newly allocated block */
		NEXT_HDR (pNewHdr)->pPrevHdr = pNewHdr;
	}
    else 
	{
		/* the extra bytes are big enough to be a fragment on the free list -
		* first, fix up the newly allocated block.
		*/
		pNewHdr->nWords = nWords;
		pNewHdr->free   = TRUE; 

		/* split off the memory after pNewHdr and add it to the free list */
		pNextHdr = NEXT_HDR (pNewHdr);
		pNextHdr->nWords = ((unsigned) endOfBlock - (unsigned) pNextHdr) / 2;
		pNextHdr->pPrevHdr = pNewHdr;
		pNextHdr->free = TRUE;

		dllAdd (&pMemPart->freeList, HDR_TO_NODE (pNextHdr));	

		/* fix next block to point to the new fragment on the free list */
		NEXT_HDR (pNextHdr)->pPrevHdr = pNextHdr;
	}

    return (pNewHdr);
}

/*
// Mem_FindMax - find the size of the largest available free block
//
// This routine searches for the largest block in the memory partition free
// list and returns its size.
//
// RETURNS: The size, in bytes, of the largest available block.
*/
int Mem_FindMax( PARTION* pMemPart )
{
    BLOCK_HDR *pHdr;
    DL_NODE *	pNode;
    unsigned	biggestWords = 0;

    /* go through free list and find largest free */
    for(pNode = DLL_FIRST (&pMemPart->freeList); pNode != NULL;
		pNode = DLL_NEXT (pNode)) 
	{
		pHdr = NODE_TO_HDR (pNode);
		if (pHdr->nWords > biggestWords)
			biggestWords = pHdr->nWords;
	}

    return (2 * biggestWords - sizeof (BLOCK_HDR));
}

/*
// Mem_Info - get partition information
//
// This routine takes a partition ID and a pointer to a PARTION_STAT structure.
// All the parameters of the structure are filled in with the current partition
// information.
//
// RETURNS: OK if the structure has valid data, otherwise ERROR.
*/
int Mem_Info( PARTION* pMemPart, PARTION_STAT* pPartStats)
{
    BLOCK_HDR* pHdr;
    DL_NODE * pNode;

    if (pMemPart == NULL || pPartStats == NULL) 
	{
		return (-1);
	}

    pPartStats->numBytesFree     = 0;
    pPartStats->numBlocksFree    = 0;
    pPartStats->maxBlockSizeFree = 0;
    pPartStats->numBytesAlloc    = 0;
    pPartStats->numBlocksAlloc   = 0;

    /* Get free memory information */
    for(pNode = DLL_FIRST (&pMemPart->freeList); pNode != NULL;
	    pNode = DLL_NEXT (pNode)) 
	{
		pHdr = NODE_TO_HDR (pNode);

	    /* check consistency and delete if not */
    	if (!Mem_Validate (pMemPart, pHdr, pHdr->free)) 
		{
	    	dllRemove (&pMemPart->freeList, HDR_TO_NODE (pHdr));
	    	return (-1);
	    } 
	    else 
		{
	    	/* All byte counts are in words, the conversion is done later */
	    	pPartStats->numBytesFree +=  pHdr->nWords;
	    	pPartStats->numBlocksFree++;
	    	if (pPartStats->maxBlockSizeFree < pHdr->nWords)
				pPartStats->maxBlockSizeFree = pHdr->nWords;
	    }
	}

    /* Get allocated memory information */
    if (pMemPart->curBlocksAllocated != 0) 
	{
		pPartStats->numBytesAlloc  = pMemPart->curWordsAllocated; 
		pPartStats->numBlocksAlloc = pMemPart->curBlocksAllocated;
	}

    /* Convert from words to bytes */
    pPartStats->numBytesFree     *= 2;
    pPartStats->maxBlockSizeFree *= 2;
    pPartStats->numBytesAlloc    *= 2;

    return (0);
}


