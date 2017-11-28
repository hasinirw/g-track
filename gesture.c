/*includes*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <linux/ctype.h>
#include <asm/hardware.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/gpio.h>
#include <asm/arch/hardware.h>
#include <asm-arm/arch-pxa/gpio.h>
#include <linux/interrupt.h>
#include <asm-arm/arch/hardware.h>

MODULE_LICENSE("GPL");

static int gesture_major = 61;
static int curr_pic = 0;
static char ibuf[150];

/*function declaration*/
static int gesture_init(void);
static void gesture_exit(void);
static ssize_t gesture_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static ssize_t gesture_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

/*fops*/
struct file_operations gesture_fops = {
	write: gesture_write,
	open: NULL,
	release: NULL,
	read: gesture_read
};

/* Declaration of the init and exit functions */
module_init(gesture_init);
module_exit(gesture_exit);


/*function definitions*/
static int gesture_init(void){
int result;
	/* Registering device */
	result = register_chrdev(gesture_major, "gesture", &gesture_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"gesture: cannot obtain major number %d\n", gesture_major);
		return result;
	}

curr_pic = 0;

	printk("gesture module loaded.\n");
	return 0;
}

static void gesture_exit(void) {
/* Freeing the major number */
	unregister_chrdev(gesture_major, "gesture");
	printk(KERN_ALERT "Removing gesture module\n");
}


static ssize_t gesture_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
char tbuf[32];
	if (*f_pos > count)
	{
		return 0;
	}

	sprintf(tbuf,"%d\n", curr_pic);
    
   count = strlen(tbuf);
    
    if (copy_to_user(buf, tbuf, count))
    {
        return -EFAULT;
    }
    
    *f_pos += count;
    return count;
}


static ssize_t gesture_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
int i;
    if (copy_from_user(ibuf, buf, count))
    {
        return -EFAULT;
    }

for (i=0; i<150; i++){
	printk(KERN_INFO "Number at %dth position is=%d\n",i,ibuf[i]);
}
    
	count = strlen(ibuf);
    *f_pos += count;
    return count;
}
