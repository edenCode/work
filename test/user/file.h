#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include <ufx.h>

/*
// File mount directory. 
*/
#define USB_MOUNT_PATH "/mnt/usb"
#define MMC_MOUNT_PATH "/mnt/mmc"

/*
// Filesystem type. 
*/
typedef enum FILESYS_TYPE
{
    FILESYS_VFAT = 0, /* vfat file system */
    FILESYS_JFFS2,    /* jffs2 file system */
	FILESYS_YAFFS2,   /* yaffs2 file system */
	FILESYS_EXT2      /* Ext2 file system */
} FILESYS_TYPE;

/*
// File system information. 
*/
typedef struct FILESYS_INFO
{
	UINT32 totalSize; /* Size in MBs */
	UINT32 totalFree; /* Free in MBs */
} FILESYS_INFO;

/* function declarations */

extern int  File_GetLine( FILE *fp, char *line, int len );
extern void File_AddLine( FILE *fp, char *line );

extern int  File_ReadAll( int hFile, char* buf, int len );
extern int  File_TestRead( int hFile, int tminms );
extern int  File_WriteAll( int hFile, char* buf, int len );
extern int  File_TestWrite( int hFile, int tminms );

extern int  File_Save( char* szName, char* buf, int len );
extern int  File_Dump( char* dirname, char* name, char* buf, int len );
extern int  File_Each( char* dirname, void(*entry)(char*, void*), void* arg );
extern int  File_GetInfo( char* name, FILESYS_INFO* pInfo );

extern int  File_DeviceMount( char* device, char* dirname, int type );
extern int  File_DeviceFormat( char* device, int type );
extern int  File_DeviceUnmount( char* device );
extern int  File_DeviceMounted( char* device );

extern int  File_MmcInserted( void );
extern int  File_MmcIsReadOnly( void );

extern int  File_UsbInserted( void );
extern int  File_UsbIsReadOnly( void );
extern int  File_UsbDeviceName( char* devname );

#endif

/* End of file */

