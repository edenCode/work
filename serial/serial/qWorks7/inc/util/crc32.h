#ifndef _CRC32_H_
#define _CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <qWorks.h>

/* 
// Dynamic create crc table switch 
*/ 
//#define DYNAMIC_CRC_TABLE 

/* function declarations */

uint32_t crc32( uint32_t crc, const char *buf, int len );
uint32_t crc32Combine( uint32_t crc1, uint32_t crc2, int len2 );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

