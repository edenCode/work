#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <ncsc.h>

#define CSL_MUX_BASE 0x48140000
#define CSL_MUX_SIZE 0x00001000

/* Modes */
#define CSL_MUX_MODE0 0x01
#define CSL_MUX_MODE1 0x02
#define CSL_MUX_MODE2 0x04
#define CSL_MUX_MODE3 0x08
#define CSL_MUX_MODE4 0x10
#define CSL_MUX_MODE5 0x20
#define CSL_MUX_MODE6 0x40
#define CSL_MUX_MODE7 0x80
#define CSL_MUX_MASK  0xFF

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
/*
 * L_PTE_MT_BUFFERABLE and L_PTE_MT_ WRITETHROUGH were introduced in 2.6.28
 * and are preferred over L_PTE_BUFFERABLE and L_PTE_CACHEABLE (which are
 * themselves going away).
 */
# define USE_PTE_MT
#else
# undef USE_PTE_MT
#endif

/*
 * The following macros control version-dependent code:
 * USE_CACHE_VOID_ARG - #define if dmac functions take "void *" parameters,
 *    otherwise unsigned long is used
 * USE_CLASS_SIMPLE - #define if Linux version contains "class_simple*",
 *    otherwise "class*" or "device*" is used (see USE_CLASS_DEVICE usage).
 * USE_CLASS_DEVICE - #define if Linux version contains "class_device*",
 *    otherwise "device*" or "class_simple*" is used (see USE_CLASS_SIMPLE
 *    usage).
 * If neither USE_CLASS_SIMPLE nor USE_CLASS_DEVICE is set, there is further
 *    kernel version checking embedded in the module init & exit functions.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
# define USE_CACHE_VOID_ARG
# undef USE_CLASS_DEVICE
# undef USE_CLASS_SIMPLE
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
# define USE_CACHE_VOID_ARG
# define USE_CLASS_DEVICE
# undef USE_CLASS_SIMPLE
#else  /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */
# define USE_CLASS_SIMPLE
# undef USE_CLASS_DEVICE
# undef USE_CACHE_VOID_ARG
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

#define NCSC_GPIO_PIN (0 * 32 + 7)

/* Linux device object */
typedef struct dev_obj {
	struct cdev   cdev; /* Char device structure    */
	struct class* class;
	int           major; /* Device driver major number */
} DEV_OBJ;

/* Locals */
static DEV_OBJ objDev;

/* Object initialize */
static int Obj_Init( void )
{
	unsigned int *base = (unsigned int*)ioremap(CSL_MUX_BASE, CSL_MUX_SIZE);
	
	if (!base) {
		printk("%s: ioremap(%08x) failed.\n", __func__, CSL_MUX_BASE);
		return -1;
	}
	
	/* AF11 --> GP0[7] */
	*(volatile unsigned int*)(base + 0x0C34) &= ~CSL_MUX_MASK;
	*(volatile unsigned int*)(base + 0x0C34) |= CSL_MUX_MODE1;
	
	iounmap(base);
	
	if (gpio_request(NCSC_GPIO_PIN, NULL))
		return -1;
	
	gpio_direction_output(NCSC_GPIO_PIN, 0);
	
	return 0;
}

/* Object Cleanup */
static void Obj_Cleanup( void )
{
	gpio_free(NCSC_GPIO_PIN);
}

/* Device open operation */
static int open(struct inode *inode, struct file *filp)
{
	return 0;
}

/* Device release operation */
static int release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* Device IO control operation */
static long ioctl( struct file *filp, unsigned int cmd, unsigned long arg )
{
	int enable;

	/* Must be NCSC commands */
	if (!NCSC_IOCTL_CMD_IS_VALID(cmd))
		return -EINVAL;
	cmd = NCSC_IOCTL_CMD_GET(cmd);
	switch (cmd) {
		case NCSC_CMD_ENABLE:
			if (copy_from_user(&enable, (void*)arg, sizeof(enable))) 
	            return -EFAULT;
	        gpio_set_value(NCSC_GPIO_PIN, !!enable);
		break;
		default: return -ENOTTY;
	}
	return 0;
}

/* File operations */
static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = open,
	.release = release,
	.unlocked_ioctl = ioctl,
};

/* Device Initialize */
static int __init Ncsc_Init(void)
{
	printk("NCSC %s, built on " __DATE__ " at " __TIME__ " ", NCSC_VERSION);
	
	/* Clear driver objects */
	memset((char*) &objDev, 0, sizeof(objDev));
	
	objDev.major = register_chrdev(0, NCSC_DRV_NAME, &fops);
	if (objDev.major<0) {
		printk("[FAILED]\n");
        return -ENODEV;
    }
#ifdef USE_CLASS_SIMPLE
    objDev.class = class_simple_create(THIS_MODULE, NCSC_DRV_NAME);
#else
    objDev.class = class_create(THIS_MODULE, NCSC_DRV_NAME);
#endif
#ifdef USE_CLASS_SIMPLE
    class_simple_device_add(objDev.class, MKDEV(objDev.major, 0), NULL, NCSC_DRV_NAME);
#else
# ifdef USE_CLASS_DEVICE
    class_device_create(objDev.class, NULL, MKDEV(objDev.major, 0), NULL, NCSC_DRV_NAME);
# else
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
    device_create(objDev.class, NULL, MKDEV(objDev.major, 0), NULL, NCSC_DRV_NAME);
#  else
    device_create(objDev.class, NULL, MKDEV(objDev.major, 0), NCSC_DRV_NAME);
#  endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
# endif // USE_CLASS_DEVICE
#endif // USE_CLASS_SIMPLE

	/* Init Object */
	Obj_Init();
	
	printk("[  OK  ]\n");

	return 0;
}

/* Device Driver Exit */
static void __exit Ncsc_Exit(void)
{
#ifdef USE_CLASS_SIMPLE
    class_simple_device_remove(MKDEV(objDev.major, 0));
    class_simple_destroy(objDev.class);
#else
# ifdef USE_CLASS_DEVICE
    class_device_destroy(objDev.class, MKDEV(objDev.major, 0));
# else
    device_destroy(objDev.class, MKDEV(objDev.major, 0));
# endif // USE_CLASS_DEVICE
    class_destroy(objDev.class);
#endif // USE_CLASS_SIMPLE

	/* Cleanup Object */
	Obj_Cleanup();
}

MODULE_LICENSE("GPL");
module_init(Ncsc_Init);
module_exit(Ncsc_Exit);

/* End of file */

