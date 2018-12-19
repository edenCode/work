#ifndef _BLIB_H_
#define _BLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

void binvtBytes( char *buf, int nbytes );
void bcopyBytes( char *source, char *destination, int nbytes );
void bcopyWords( char *source, char *destination, int nwords );
void bcopyLongs( char *source, char *destination, int nlongs );
void bfillBytes( char *buf, int nbytes, int ch );

#ifdef __cplusplus
}
#endif

#endif 

/* End of file */
