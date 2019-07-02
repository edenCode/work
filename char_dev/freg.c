#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/device.h>
#include <linux/uaccess.h>
//#include <asm-generic/uaccess.h>

#include "freg.h"

static int freg_major = 0;
static int freg_minor = 0;

static struct class * freg_class = NULL;
static struct freg_cdev_t * freg_dev;


static int freg_open(struct inode * inode,struct file * filp);
static int freg_close(struct inode * inode,struct file * filp);
static ssize_t freg_read(struct file * filp,char __user * buf,size_t count,loff_t * f_pos);
static ssize_t freg_write(struct file * filp,const char __user * buf,size_t count,loff_t * f_pos);

/*

struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*setfl)(struct file *, unsigned long);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
			u64);
	ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
			u64);
} __randomize_layout;
*/
static struct file_operations freg_fops = {
    .owner = THIS_MODULE,
    .open  = freg_open,
    .release = freg_close,
    .read    = freg_read,
    .write   = freg_write
};

//static ssize_t freg_val_show(struct device * dev,struct device_attribute * attr,char * buf);
//static ssize_t freg_val_store(struct device * dev,struct device_attribute * attr,const char * buf,ssize_t count);


//struct device_attribute 

static int freg_open(struct inode * inode,struct file * filp)
{
    struct freg_cdev_t * dev;

    dev = container_of(inode->i_cdev,struct freg_cdev_t,dev);

    filp->private_data  = dev;

    return 0;
}


static int freg_close(struct inode * inode,struct file * filp)
{
    pr_debug_once("cdev ptr  %p \n",filp->private_data);

    return 0;
}


static ssize_t freg_read(struct file * filp,char __user * buf,size_t count,loff_t * f_pos)
{
    ssize_t err = 0;
    struct freg_cdev_t * dev  = filp->private_data;

    if(down_interruptible(&(dev->sem))){

        return -ERESTARTSYS;
    }

    if(count < sizeof(dev->val)){
        err = -EFAULT;
        goto out;
    }

    
    if(copy_to_user(buf,&(dev->val),sizeof(dev->val))){
        err = -EFAULT;
        goto out;
    }

    err = sizeof(dev->val);
    out:
        up(&(dev->sem));
        return err;
}

static ssize_t freg_write(struct file * filp,const char __user * buf,size_t count,loff_t * f_pos)
{
    ssize_t err = 0;
    struct freg_cdev_t * dev = filp->private_data;

    if(down_interruptible(&(dev->sem))){
        return -ERESTARTSYS;
    }

    if(count < sizeof(dev->val)){
        err = -EFAULT;
        goto out;
    }

    if(copy_from_user(&(dev->val),buf,sizeof(dev->val))){
        err = -EFAULT;
        goto out;
    }

    err = sizeof(dev->val);
    out:
        up(&(dev->sem));
        return err;
}

//devfs
static ssize_t __freg_get_val(struct freg_cdev_t * dev,char * buf)
{
    int val = 0;

    if(down_interruptible(&(dev->sem))){
        return -ERESTARTSYS;
    }
    val = dev->val;

    up(&(dev->sem));
    return snprintf(buf,PAGE_SIZE,"%d\n",val);
}

static ssize_t __freg_set_val(struct freg_cdev_t * dev,const char * buf,size_t count)
{
    int val = 0;

    val = simple_strtol(buf,NULL,10);

    if(down_interruptible(&(dev->sem))){
        return -ERESTARTSYS;
    }
    dev->val = val;
    up(&(dev->sem));

    return count;
}

/*
 ** interface for exporting device attributes 
struct device_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct device *dev, struct device_attribute *attr,
			char *buf);
	ssize_t (*store)(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);
};
*/
static ssize_t freg_val_show(struct device * dev,struct device_attribute * att ,char * buf)
{
    struct freg_cdev_t * hdev = (struct freg_cdev_t *)dev_get_drvdata(dev);

    return __freg_get_val(hdev,buf);
}

static ssize_t freg_val_store(struct device * dev,struct device_attribute * attr,const char * buf,size_t count)
{
    struct freg_cdev_t * cdev = (struct freg_cdev_t *)dev_get_drvdata(dev);
    return __freg_set_val(cdev,buf,count);
}


static DEVICE_ATTR(val,S_IRUGO | S_IWUSR,freg_val_show,freg_val_store);
//proc fs
#ifdef LINUX_2_6_11
static int freg_proc_read(char * page,char ** start,off_t off,int count,int * eof,void * data)
{
    if(off > 0)
    {
        *eof = 1;
        return 0;
    }
    pr_debug_once("%s[%d] data : %ld \n",__fuction__,__LINE__,simple_strtol(data,NULL,10));
    return __freg_get_val(freg_dev,page);
}

static int freg_proc_write(struct file * filp,const char __user * buf,unsigned long len,void * data)
{
    int err = 0;
    char * page = NULL;

    if(len > PAGE_SIZE)
    {
        pr_debug("%s[%d] The data is too large \n",__function__,__LINE__);
        return -EFAULT;
    }

    page = __get_free_page(GFP_KERNEL);
    if(!page){
        pr_debug("__get_free_page failed \n");
        return -EFAULT;
    }

    if(copy_from_user(page,buf,len)){
        pr_debug("copy_frome_ user failed \n");
        err = -EFAULT;
        goto out;
    }

    err = __freg_set_val(freg_dev,page,len);
    out:
        free_page(page);
        return err;
}

static void freg_proc_create(void)
{
    struct proc_dir_entry * entry;

    entry = create_proc_entry(FREG_DEVICE_NAME,0,NULL);
    if(!entry){
        pr_debug(" %s proc dir create failed \n",FREG_DEVICE_NAME);
        return;
    }

    entry->owner = THIS_MODULE;
    entry->read_proc = freg_proc_read;
    entry->write_proc = freg_proc_write;
} 
#else // linux 版本大于3.0 之后 procfs 文件系统创建方式不一样了

static void * freg_seq_start(struct seq_file * m,loff_t * pos)
{
    pr_debug_once(" seq_file  size %d ,count %d, index %d ,pos :  %d",m->size,m->count,m->index,*pos);
    
    return &freg_dev; // void * v
}

static void freg_seq_stop(struct seq_file * m,void * v)
{
    pr_debug_once(" seq_file  size %d ,count %d, index %d",m->size,m->count,m->index);

}

static void * freg_seq_next(struct seq_file * m,void * v,loff_t * pos)
{

    return NULL;
}

static int freg_seq_show(struct seq_file * m,void * v)
{
    int *val = (int *)val;
    seq_printf(m,"This device val %d \n",*val);
    return 0;
}

/*
struct seq_operations {
	void * (*start) (struct seq_file *m, loff_t *pos);
	void (*stop) (struct seq_file *m, void *v);
	void * (*next) (struct seq_file *m, void *v, loff_t *pos);
	int (*show) (struct seq_file *m, void *v);
};
*/
static struct seq_operations freg_seq_ops = {
    .start = freg_seq_start,
    .stop  = freg_seq_stop,
    .next  = freg_seq_next,
    .show  = freg_seq_show
};

static int freg_proc_open(struct inode * inode,struct file * filp)
{
    //struct proc_dir_entry * entry = proc_create()
    return single_open(filp,freg_seq_show,PDE_DATA(inode));
    //return seq_open(filp,&freg_seq_ops);
}

//ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
static ssize_t freg_proc_write(struct file * filp,const char __user * buf,size_t count,loff_t * f_pos)
{
    int err = -EFAULT;
    int val;
    struct seq_file * seq = (struct seq_file *)filp->private_data;
    
    pr_debug_once("freg_proc_write count %d \n",count);
    if(count > 0){
        if(get_user(val,buf)){
            return -EFAULT;
        }
        err = seq_write(seq,val,sizeof(val));
        if(err < 0)
            return err;
    }
    return err;
}

static struct file_operations freg_proc_ops = {
    .owner = THIS_MODULE,
    .open  = freg_proc_open,
    .read  = seq_read,
    .write = freg_proc_write,
    .llseek = seq_lseek,
    .release = single_release
};

static void create_proc_freg(void)
{
    struct proc_dir_entry * entry = proc_create(FREG_DEVICE_NAME,0666,NULL,&freg_proc_ops);
}

static void remove_proc_freg(void)
{
    remove_proc_entry(FREG_DEVICE_NAME,NULL);
}
#endif

static int __freg_setup(struct freg_cdev_t * dev)
{
    int err = 0;
    dev_t devno = MKDEV(freg_major,freg_minor);

    memset(dev,0,sizeof(struct freg_cdev_t));
    
    cdev_init(&(dev->dev),&freg_fops);
    dev->dev.owner = THIS_MODULE;
    dev->dev.ops = &freg_fops;

    err = cdev_add(&(dev->dev),devno,1);
    if(err){
        return err;
    }
    
    //
    sema_init(&dev->sem,1);
    dev->val = 0;

    return 0;
}

static int __init freg_init(void)
{
    int err = -1;
    dev_t dev = 0;
    struct device * device = NULL;

    //pr_debug_once("freg_init freg device \n");
    printk(KERN_DEBUG"freg_init start \n");
    err = alloc_chrdev_region(&dev,0,1,FREG_DEVICE_NAME);
    if(err < 0){
        pr_debug_once("alloc chrdev region failed \n");
        goto out;
    }

    freg_major = MAJOR(dev);
    freg_minor = MINOR(dev);

    freg_dev = kmalloc(sizeof(struct freg_cdev_t),GFP_KERNEL);
    if(!freg_dev){
        err = -ENOMEM;
        pr_debug_once(KERN_ALERT"Failed to kmalloc freg device \n");
        goto unregister;
    }

    err = __freg_setup(freg_dev);
    if(err){
        pr_debug_once(KERN_ALERT"__freg_setup failed \n");
        goto cleanup;
    }

    freg_class = class_create(THIS_MODULE,FREG_DEVICE_NAME);
    if(IS_ERR(freg_class)){
        err = PTR_ERR(freg_class);
        pr_debug_once(KERN_ALERT"class create freg_class failed \n");
        goto destroy_cdev;
    }
    
    device = device_create(freg_class,NULL,dev,NULL,"%s",FREG_DEVICE_NAME);
    if(IS_ERR(device)){
        pr_debug_once(KERN_ALERT"device_create failed \n");
        goto destroy_class;
    }

    err = device_create_file(device,&dev_attr_val);
    if(err < 0){
        pr_debug_once(KERN_ERR"device_create_file failed  err = %ld \n",err);
        goto destroy_device;
    }

    dev_set_drvdata(dev,freg_dev);

    //proc fs 
    create_proc_freg();

    pr_debug_once(KERN_DEBUG"freg_init successfuel \n");


    return 0;
destroy_device:
    device_destroy(freg_class,dev);
destroy_class:
    class_destroy(freg_class);
destroy_cdev:
    cdev_del(&(freg_dev->dev));
cleanup:
    kfree(freg_dev);
unregister:
    unregister_chrdev_region(MKDEV(freg_major,freg_minor),1);
out:
    return err;
}

static void __exit freg_exit(void)
{
    dev_t dev = MKDEV(freg_major,freg_minor);

    remove_proc_freg();

    if(freg_class){
        device_destroy(freg_class,dev);
        class_destroy(freg_class);
    }

    if(freg_dev){
        cdev_del(&(freg_dev->dev));
        kfree(freg_dev);
    }

    unregister_chrdev_region(dev,1);
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Freg cdev driver");

module_init(freg_init);
module_exit(freg_exit);