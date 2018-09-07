#ifndef _NAND_H_
#define _NAND_H_

/* function declarations */

int Nand_Read ( char *mtdname, unsigned long offset, char* buf, int len );
int Nand_Write( char *mtdname, unsigned long offset, char* buf, int len );
int Nand_Erase( char *mtdname );

#endif

/* End of file */

