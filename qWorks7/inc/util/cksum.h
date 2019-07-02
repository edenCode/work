#ifndef _CKSUM_H_
#define _CKSUM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>

/* function declarations */

uint8_t crc8( uint8_t crc, uint8_t *buf, int count );
uint8_t sum8( uint8_t* buf, int len );
uint16_t sum16( uint16_t* buf, int len );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
