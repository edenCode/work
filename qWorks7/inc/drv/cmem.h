#ifndef _CMEM_H_
#define _CMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>

/* function declarations */

int Cmem_Init( unsigned long base, int size, int flag );
int Cmem_Cleanup( void );

uint8_t* Cmem_Alloc( int nBytes, int align, int flag );
int Cmem_Free( uint8_t* addr, int size );

unsigned long Cmem_GetPhys( uint8_t* addr );

void Cmem_CacheWb( uint8_t* addr, int size );
void Cmem_CacheInv( uint8_t* addr, int size );
void Cmem_CacheWbInv( uint8_t* addr, int size );

#ifdef __cplusplus
}
#endif

#endif  /* _CMEM_H_ */

