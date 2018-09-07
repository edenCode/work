#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/select.h>
#include <osFile.h>
#include <osError.h>

#define LF 10
#define CR 13

/* 
// System files
*/
#define MMC_WRPROT_PATH	"/sys/block/mmcblk0/ro"
#define MMC_INSERT_PATH	"/sys/block/mmcblk0/size"

#define USB_WRPROT_PATH	"/sys/block/sda/ro"
#define USB_INSERT_PATH	"/sys/block/sda/size"

/*
// OSFile_GetLine - Get one line from file. 
// 
// RETURNS: 1 - OK, 0 - EOF.
*/
int OSFile_GetLine( FILE *f, char *s, int n ) 
{
    int i = 0;

    while(1) 
	{
        s[i] = (char)fgetc(f);
        if( s[i] == CR) s[i] = fgetc(f);
        if((s[i] == 0x4) || (s[i] == LF) || (i == (n-1))) 
		{
            s[i] = '\0';
            return (feof(f) ? 0 : 1);
        }
        ++ i;
    }
}

/*
// OSFile_AddLine - Add one line to file. 
// 
// RETURNS: N/A.
*/
void OSFile_AddLine( FILE *f, char *line ) 
{
    int x;

    for(x=0; line[x]; x++) 
		fputc(line[x], f);
    fputc('\n',f);
}

/*
// OSFile_TestWrite - Check if a fd is writeable in specific time. 
//  
// RETURNS: 0 - Not Writeable, 1 - Writeable.
*/
int OSFile_TestWrite( int fd, int tminms )
{
	fd_set  fdSet;
	struct 	timeval tm;
	struct 	timeval *ptv = (struct timeval*)NULL;

	FD_ZERO(&fdSet );
	FD_CLR (fd, &fdSet);
	FD_SET (fd, &fdSet);
	
    /* Converts the time to timeval */
	if (tminms>=0) 
	{
		tm.tv_sec  = tminms / 1000;
		tm.tv_usec = tminms % 1000 * 1000;
		ptv = &tm;
	}
    /* Polls the file descriptor */
    if (select(fd + 1, NULL, &fdSet, NULL, ptv)>0) 
	{
        return (FD_ISSET(fd, &fdSet)) ? 1 : 0;
    }
    return 0;
}

/*
// OSFile_TestRead - Check if a fd is readable in specific time.
//  
// RETURNS: 0 - Not readable, 1 -Readable.
*/
int OSFile_TestRead( int fd, int tminms )
{
	fd_set  fdSet;
	struct 	timeval tm;
	struct 	timeval *ptv = (struct timeval*)NULL;

	FD_ZERO(&fdSet);
	FD_CLR (fd, &fdSet);
	FD_SET (fd, &fdSet);
	
    /* Converts the time to timeval */
	if (tminms>=0) 
	{
		tm.tv_sec  = tminms / 1000;
		tm.tv_usec = tminms % 1000 * 1000;
		ptv = &tm;
	}
    /* Polls the file descriptor */
    if (select(fd + 1, &fdSet, NULL, NULL, ptv)>0) 
	{
        return (FD_ISSET(fd, &fdSet)) ? 1 : 0;
    }
    return 0;
}

/*
// OSFile_WriteAll - Write specific data to file.
// 
// RETURNS: bytes written, -1 - ERROR.
*/
int OSFile_WriteAll( int fd, char* buf, int len )
{
	char *ptr = buf;
	int nwrite, nleft, total;

	nwrite = 0; nleft = len; total = 0;
	while (nleft>0) 
	{
		nwrite = write(fd, ptr, nleft);
		if(nwrite<=0) break;
		nleft -= nwrite; ptr += nwrite; total += nwrite;
	}
	return (nleft > 0) ? (-1) : total;
}

/*
// OSFile_ReadAll - Read specific data from file.
// 
// RETURNS: bytes read, -1 - ERROR.
*/
int OSFile_ReadAll( int fd, char* buf, int len )
{
	char *ptr = buf;
	int nread, nleft, total;

	nread = 0; nleft = len; total = 0;
	while (nleft>0) 
	{
		nread = read(fd, buf, nleft);
		if (nread<=0) break;
		nleft -= nread; ptr += nread; total += nread;
	}
	return (nleft > 0) ? (-1) : total;
}

/* 
// OSFile_Save - Save file to Storage. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int OSFile_Save( char* name, char* buf, int len )
{
	int fd = -1;

	/* Create a file */
	fd = open(name, O_RDWR | O_CREAT);
	if (fd<0) return -1;

	/* Write to file */
	if (OSFile_WriteAll(fd, buf, len) != len) 
	{
		close(fd);
		remove(name);
		return -1;
	}
	fsync(fd);
	close(fd);
	return 0;
}

/* 
// search - Search file at specific directory. 
//
// RETURNS: 0 : Not found, 1 : found.
*/
static int search( char* dirname, char* name )
{
	struct dirent *dp;
	DIR *dirp;
	struct stat st;

	/* Open directory to search */
	dirp = opendir(dirname);
	if (dirp == NULL) return 0;
	dp = readdir(dirp);
	while (dp != NULL)	
	{
		/* Ignore . and .. directory */
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
		{
			sprintf(name, "%s/%s", dirname, dp->d_name);
			if (!stat(name, &st)) 
			{
				/* Only store regular file */
				if (S_ISREG(st.st_mode))
				{
					closedir(dirp);
					return 1;
				}
				if (S_ISDIR(st.st_mode))	
				{
					return search(name, name);
				}
			}
		}
		dp = readdir(dirp);
	}
	closedir(dirp);
	return 0;
}

/* 
// OSFile_Dump - Dump file from Storage. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int OSFile_Dump( char* dir, char* name, char* buf, int len )
{
	struct stat st;
	int    fd = -1;

	/* Search directory. */
	if (!search(dir, name )) 
	{
		return -1;
	}
	/* Check buffer */
	if (lstat(name, &st) == -1) return -1;
	if (st.st_size > len) 
	{
		remove(name);
		return -1;
	}
	/* Open a file */
	fd = open(name, O_RDWR);
	if (fd < 0) return -1;
	/* Read from file */
	if (OSFile_ReadAll(fd, buf, st.st_size) != st.st_size) 
	{
		close(fd);
		return -1;
	}
	/* Close and remove the file */
	fsync(fd);
	close(fd);
	return (int)st.st_size;
}

/* 
// OSFile_Each- List file on Storage. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int OSFile_Each( char* dirname, void(*eachFxn)(char*, void*), void* arg )
{
    DIR *dirp;
	struct stat st;
	char path[256];
	struct dirent *dp;

	/* Open directory to search */
	if ((dirp=opendir(dirname))==NULL)
	{
		return -1;
	}
	dp = readdir(dirp);
	while (dp != NULL)	
	{
		/* Ignore . and .. directory */
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
		{
			sprintf(path, "%s/%s", dirname, dp->d_name);
			if (!lstat(path, &st)) 
			{
				/* Regular file */
				if (S_ISREG(st.st_mode))
				{
					eachFxn(path, arg);
				}
				/* Directory */
				if (S_ISDIR(st.st_mode))	
				{
					if (OSFile_Each(path, eachFxn, arg))
					{
						closedir(dirp);
						return -1;
					}
				}
			}
		}
		dp = readdir(dirp);
	}
	closedir(dirp);
	return 0;
}

/* 
// OSFile_GetInfo - Get Storage information. 
//
// RETURNS: 0 : success, !0 : failed.
*/
int OSFile_GetInfo( char* name, VFS_INFO* pInfo )
{
	struct statvfs buf;

	if (statvfs(name, &buf)==-1) return -1;
	pInfo->totalSize = (buf.f_bsize / 512) * buf.f_blocks / 2048;
	pInfo->totalFree = (buf.f_bsize / 512) * buf.f_bfree  / 2048;
	return 0;
}

/* End of file */
