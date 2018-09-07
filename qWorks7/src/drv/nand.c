#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <asm/types.h>
#include "mtd/mtd-user.h"

#include <qWorks.h>

/*
 * Read data from flash at an offset into a provided buffer. On NAND it skips
 * bad blocks but makes sure it stays within starting from. On NOR the loop is only run once.
 */
int Nand_Read( char *mtdname, unsigned long offset, char* buf, int len )
{
	size_t processed = 0; /* progress counter */
	size_t readlen = len; /* current read length */
	off_t  block_seek;	/* offset inside the current block to the start of the data */
	loff_t blockstart;	/* running start of the current block - MEMGETBADBLOCK needs 64 bits */
	struct mtd_info_user meminfo;
	int fd, ret = 0;
	
	/* Open the MTD device for operation */
	fd = open(mtdname, O_RDONLY);
	if (fd==-1) return -1;

	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo)) 
	{
		close(fd);
		return -1;
	}
	/* Check, if length fits into device */
	if (len>meminfo.size-offset)
	{
		close(fd);
		return -1;
	}
	/*
	 * Start of the first block to be read, relies on the fact, that
	 * erase sector size is always a power of 2
	 */
	blockstart = offset & ~(meminfo.erasesize - 1);

	/* Offset inside a block */
	block_seek = offset - blockstart;

	/* This only runs once on NOR flash */
	while (processed<len) 
	{
		ret = ioctl(fd, MEMGETBADBLOCK, &blockstart);
		if (ret<0) 
		{
			close(fd);
			return -1;
		}
		if (ret==1) 
		{ 
			/* block is bad */
			blockstart += meminfo.erasesize;
			continue;
		}
		/*
		 * If a block is bad, we retry in the next block at the same
		 * offset - see common/env_nand.c::writeenv()
		 */
		lseek (fd, blockstart + block_seek, SEEK_SET);
		ret = read (fd, buf + processed, readlen);
		if (ret != readlen) 
		{
			return -1;
		}
		processed += readlen;
		readlen = MIN(meminfo.erasesize, len - processed);
		block_seek = 0;
		blockstart += meminfo.erasesize;
	}
	return processed;
}

/* Do kernel flash */
int Nand_Write( char *mtdname, unsigned long offset, char* buf, int len )
{
	loff_t offs;
	char* pdata = buf;
	struct mtd_info_user meminfo;
	int fd, writelen, ret=0;
	int blockstart = -1;
	
	/* Open the MTD device for operation */
	fd = open(mtdname, O_RDWR);
	if (fd==-1) return -1;

	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo)) 
	{
		close(fd);
		return -1;
	}
	/* Check, if length fits into device */
	if (len>meminfo.size-offset)
	{
		close(fd);
		return -1;
	}
	/* Must be write page aligned */
	if (len % meminfo.writesize)
	{
		close(fd);
		return -1;
	}
	writelen = len;
	/* Get data from input and write to the device */
	while (writelen && (offset<meminfo.size)) 
	{
		while (blockstart!=(offset & (~meminfo.erasesize + 1))) 
		{
			blockstart = offset & (~meminfo.erasesize + 1);
			offs = blockstart;

		    /* Check all the blocks in an erase block for bad blocks */
			do {
			   	if ((ret = ioctl(fd, MEMGETBADBLOCK, &offs))<0) 
				{
					close(fd);
					return -1;
				}
				if (ret==1) 
				{
					offset = blockstart + meminfo.erasesize;
				}
			    offs +=  meminfo.erasesize;
			} while (offs < blockstart + meminfo.erasesize);
		}
		/* Write out the Page data */
		if (pwrite(fd, pdata, meminfo.writesize, offset) != meminfo.writesize) 
		{
			close(fd);
			return -1;
		}
		pdata  += meminfo.writesize;
		writelen -= meminfo.writesize;
		offset += meminfo.writesize;
	}
	
	return 0;
}

int Nand_Erase( char *mtdname )
{
	char cmd[128];
	
	sprintf(cmd, "/usr/sbin/flash_eraseall %s >> /dev/null\n", mtdname);
	
	return system(cmd) ? (-1) : 0;
}

/* End of file */
