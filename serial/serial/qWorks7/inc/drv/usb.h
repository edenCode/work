#ifndef _USB_H_
#define _USB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

int Usb_Mount( char* device, char* dir, char* fstype );
int Usb_Unmount( char* device );
int Usb_Mounted( char* device );
int Usb_Inserted( void );
int Usb_ReadOnly( void );
int Usb_DeviceName( char* name );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */

