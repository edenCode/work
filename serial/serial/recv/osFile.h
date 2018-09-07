#ifndef _OSFILE_H_
#define _OSFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "qWorks.h"

/*
// File system information. 
*/
typedef struct VFS_INFO
{
	uint32_t totalSize; /* Size in MBs */
	uint32_t totalFree; /* Free in MBs */
} VFS_INFO;

/* function declarations */

int  OSFile_GetLine( FILE *fp, char *line, int len );
void OSFile_AddLine( FILE *fp, char *line );

int OSFile_ReadAll( int hFile, char* buf, int len );
int OSFile_TestRead( int hFile, int tminms );
int OSFile_WriteAll( int hFile, char* buf, int len );
int OSFile_TestWrite( int hFile, int tminms );

int OSFile_Save( char* name, char* buf, int len );
int OSFile_Dump( char* dir, char* name, char* buf, int len );
int OSFile_Each( char* dir, void(*entry)(char*, void*), void* arg );

int OSFile_GetInfo( char* name, VFS_INFO* pInfo );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

