#ifndef __FREG__
#define __FREG__

#include <linux/cdev.h>
#include <linux/semaphore.h>

#define FREG_DEVICE_NAME "freg"

struct freg_cdev_t{
    int val;
    struct semaphore sem;
    struct cdev dev;
};

#endif // !__FREG__