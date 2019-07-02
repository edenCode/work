
extern deviceBootInfo_t supportedDevices[];
void usb_library_load();
void usb_library_unload();
usbBootError_t usb_find_device(unsigned idx, char *addr, unsigned addrsize, void **device, int vid, int pid)