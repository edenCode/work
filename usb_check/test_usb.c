#include<stdio.h>
#include<libusb.h>
//#include  <libusb-1.0/libusb.h>  
#include<sys/types.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
 //#include "libusbi.h"
 

static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
	 
	 
}



int get_configuration(libusb_device* dev, struct libusb_config_descriptor *config)
{
   int ret = 0;
   ret = libusb_get_config_descriptor(dev, 0, &config);
   return ret;
}
 
static void dump_altsetting(libusb_device_handle *dev, const struct libusb_interface_descriptor *interface)
{
//	char cls[128], subcls[128], proto[128];
//	char *ifstr;
 
//	get_class_string(cls, sizeof(cls), interface->bInterfaceClass);
//	get_subclass_string(subcls, sizeof(subcls), interface->bInterfaceClass, interface->bInterfaceSubClass);
//	get_protocol_string(proto, sizeof(proto), interface->bInterfaceClass, interface->bInterfaceSubClass, interface->bInterfaceProtocol);
//	ifstr = get_dev_string(dev, interface->iInterface);
 
	printf("    Interface Descriptor:\n"
	       "      bLength             %5u\n"
	       "      bDescriptorType     %5u\n"
	       "      bInterfaceNumber    %5u\n"
	       "      bAlternateSetting   %5u\n"
	       "      bNumEndpoints       %5u\n"
	       "      bInterfaceClass     %5u\n"
	       "      bInterfaceSubClass  %5u\n"
	       "      bInterfaceProtocol  %5u\n",
	       interface->bLength, interface->bDescriptorType, interface->bInterfaceNumber,
	       interface->bAlternateSetting, interface->bNumEndpoints, interface->bInterfaceClass,
	       interface->bInterfaceSubClass,interface->bInterfaceProtocol);
 
//	free(ifstr);
}
 
static void dump_interface(libusb_device_handle *dev, const struct libusb_interface *interface)
{
	int i;
 
	for (i = 0; i < interface->num_altsetting; i++)
		dump_altsetting(dev, &interface->altsetting[i]);
}
 
static int list_devices(libusb_context *ctx,libusb_hotplug_callback_handle * handle,libusb_hotplug_callback_handle * handle)
{
	libusb_device **list;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor* conf;
 
	libusb_device_handle *  handle = NULL;
	int config= 0;
	int ret;
 
	int status;
	ssize_t num_devs, i, j, k;
 
	status = 1; /* 1 device not found, 0 device found */
 
	num_devs = libusb_get_device_list(ctx, &list);
	if (num_devs < 0)
		goto error;
	printf(" find %d usb devices \n",num_devs);
	for (i = 0; i < num_devs; ++i)
	{
		libusb_device *dev = list[i];
		libusb_open(dev,&handle);
		if(!handle) 
		{
			printf(" open failed \n");
			continue;
		}
		
		libusb_get_configuration(handle,&config);
		//printf("libusb_get_configuration \n");
 		uint8_t bnum = libusb_get_bus_number(dev);
		//printf("libusb_get_bus_number \n");
		uint8_t dnum = libusb_get_device_address(dev);
		//printf("libusb_get_device_address \n");
 
		libusb_get_device_descriptor(dev, &desc);
		//printf("libusb_get_device_descriptor \n");
		status = 0;
		printf("device:%04x:%04x\n",desc.idVendor,desc.idProduct);
		printf("bDeviceSubClass = %5u\n",desc.bDeviceSubClass);
		printf("bDeviceClass    = %5u\n",desc.bDeviceClass);
		printf("bDeviceProtocol    = %5u\n",desc.bDeviceProtocol);
		for( j = 0; j < desc.bNumConfigurations; ++j)
		{
             
			 ret = libusb_get_config_descriptor(dev, j, &conf);
                        
			 if(ret)
			 {
                      fprintf(stderr, "Couldn't get configuration "
                                                "descriptor %lu, some information will "
                                                "be missing\n", j);
             } 
			 else
			 {
				printf("bNumberInterfaces = %5u\n",conf->bNumInterfaces);
				printf("bConfigurationValue = %5u\n",conf->bConfigurationValue);
				
				for (k = 0 ; k < conf->bNumInterfaces ; k++)
					dump_interface(handle, &conf->interface[k]);
                
				libusb_free_config_descriptor(conf);
             }
        }
				
		ret = libusb_hotplug_register_callback (NULL,LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 0,desc.idVendor,desc.idProduct,desc.bDeviceClass, hotplug_callback, NULL,handle[i]);
		if(ret != LIBUSB_SUCCESS)
		{
			fprintf(stderr,"libusb_hotplug_register_callback LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED err");
		}
		ret = libusb_hotplug_register_callback(NULL,LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0,desc.idVendor,desc.idProduct,desc.bDeviceClass, hotplug_callback, NULL,handle[i]);	
		if(ret != LIBUSB_SUCCESS)
		{
			fprintf(stderr,"libusb_hotplug_register_callback LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT err");
		}
		printf(" \n");
	}
	
	libusb_free_device_list(list, 0);
error:
	return status;
}
 
int main(int argc, char* args[])
{
   
   int err = 0;
   libusb_context *ctx;
   libusb_hotplug_callback_handle hp[10];
   libusb_hotplug_callback_handle hp1[10];
   err = libusb_init(&ctx);
   
   if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG))
   {
		printf ("Hotplug capabilites are not supported on this platform\n");
		libusb_exit (NULL);
		return EXIT_FAILURE;
   }
	
	
   if(err)
   {
	fprintf(stderr, "unable to initialize libusb: %i\n", err);
	return EXIT_FAILURE;
   }
   
   printf(" find usb list ... \n");
   list_devices(ctx,hp,hp1);
 
return 0;
}
