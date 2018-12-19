#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mntent.h>
#include <sys/mount.h>
#include <osError.h>
#include <usb.h>

/* 
// System files
*/
#define USB_WRPROT_PATH	"/sys/block/sda/ro"
#define USB_INSERT_PATH	"/sys/block/sda/size"

/* 
// Usb_Mount - Mount Storage. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int Usb_Mount( char* device, char* dir, char* fstype )
{
	char buf[256];
	int status = 0;
	
	/* Check directory */
	if (access(dir, F_OK)) 
	{
		OSError_Set(errno);
		return -1;
	}
	/* Check device */
	if (access(device, F_OK)) 
	{
		OSError_Set(errno);
		return -1;
	}
	/* Check mount state */
	if (Usb_Mounted(device)) return 0;
	
	/* Mount filesystem according to type */
	sprintf(buf, "/bin/mount -t %s %s %s > /dev/null\n", fstype, device, dir);
	status = system(buf);
	system("sync\n");
	
	return status ? -1 : 0;
}

/* 
// Usb_Mounted - Check if Storage is mounted. 
//
// RETURNS: 1 : mounted, 0 : Not mounted.
*/
int Usb_Mounted( char* device )
{
	FILE *fp = NULL;
	struct mntent *mnt;
	int mounted = 0;
	
	if((fp=setmntent(MOUNTED, "r"))==NULL)
	{
		return 0;
	}
	while((mnt=getmntent(fp))!=NULL) 
	{
		if(strcmp(device, mnt->mnt_fsname)==0)
		{
			mounted = 1;
		}
	}
	endmntent(fp);
	
	return mounted;
}

/* 
// Usb_Unmount - Unmount Storage. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int Usb_Unmount( char* device )
{
	FILE *fp = NULL;
	struct mntent *mnt;
	
	if((fp=setmntent(MOUNTED, "r"))==NULL)
	{
		OSError_Set(errno);
		return -1;
	}
	/* Un-mount all mounted directory */
	while((mnt=getmntent(fp)) != NULL) 
	{
		if(strcmp(device, mnt->mnt_fsname)==0)
		{
			if (umount(mnt->mnt_dir))
			{
				OSError_Set(errno);
				return -1;
			}
		}
	}
	endmntent(fp);
	return 0;
}

/* 
// Usb_Inserted - Check USB is inserted. 
//
// RETURNS: 0 : NG, 1 : OK.
*/
int Usb_Inserted( void )
{
	int retval = 0;
	FILE *fp = NULL;

	fp = fopen(USB_INSERT_PATH, "rb");
	if (fp) 
	{
		retval = 1;
		fclose(fp);
	}

	return retval;
}

/* 
// Usb_ReadOnly - Check if USB is read-only. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int Usb_ReadOnly( void )
{
	char data[5];
	int fd;

	fd = open(USB_WRPROT_PATH, O_RDONLY);
	if (fd==-1) return 0;
	read(fd, data, 1);
	close(fd);

	return (data[0]=='1') ? 1 : 0;
}

/* 
// Usb_DeviceName - Get USB is Device name. 
//
// RETURNS: Name of USB device.
*/
int Usb_DeviceName( char* devname )
{
	int i;

	for (i=0; i<32; i++)
	{
		sprintf(devname, "/dev/sda%d", i);
		if (!access(devname, F_OK))
			break;
	}
	
	return (i<32) ? 1 : 0;
}

/* End of file */

