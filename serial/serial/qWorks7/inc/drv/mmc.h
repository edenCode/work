#ifndef _MMC_H_
#define _MMC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

int Mmc_Mount( char* device, char* dir, char* fstype );
int Mmc_Unmount( char* device );
int Mmc_Mounted( char* device );
int Mmc_Inserted( void );
int Mmc_ReadOnly( void );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

