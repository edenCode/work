#ifndef _DRV_LIB_H_
#define _DRV_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Driver functions */
typedef int (*DRV_FXN)( void* );
typedef int (*IOC_FXN)( void*, int, void* );

/* Driver object */
typedef struct DRV_OBJ
{
	void*   pModObj;  /* Module object pointer */
	DRV_FXN initFxn;  /* Device initialize routine */
	DRV_FXN exitFxn;  /* Device cleanup routine */
	DRV_FXN openFxn;  /* Device cleanup routine */
	DRV_FXN closeFxn; /* Device cleanup routine */
	IOC_FXN ioctlFxn; /* Device ioctl routine */
} DRV_OBJ;

/* function declarations */

int Drv_Init ( void );
int Drv_Exit ( void );
int Drv_Open ( int hndl );
int Drv_Close( int hndl );
int Drv_Ioctl( int hndl, int cmdId, void *args );

#ifdef __cplusplus
}
#endif

#endif  /* _DRV_LIB_H_ */

/* End of file */
