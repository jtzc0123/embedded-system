#define MODULE
#define _KERNEL_
#include<linux/kernel.h>
#include <linux/module.h>
#include<linux/fs.h>
#include<linux/io.h>
#include<asm/uaccess.h>
#define MAJOR 222
#define NAME "8253"
#define SUCCESS 0
#define INIT 1
#define START 2
#define FRE 3
#define STOP 4
int Device_Open = 0;
int init_module(void);
void cleanup_module(void);
static int dopen(struct inode *inode, struct file *file);
static int dclose(struct inode *inode, struct file * file);
int dioctl(struct inode * inode, struct file * file, unsigned int
	cmd, unsigned long arg);
static struct file_operations fops =
{
.open = dopen,
.release = dclose,
.ioctl = dioctl
};
int init_module(void)
{
	int ret;
	ret = register_chrdev(MAJOR, NAME, &fops);
	if (ret < 0)
	{
		printk("<1>register error!");
		return -1;
	}
	return SUCCESS;
}
void cleanup_module(void)
{
	unregister_chrdev(MAJOR, NAME);
	printk("<1>goodbye 8253");
}
static int dopen(struct inode *inode, struct file *file)
{
	if (Device_Open)
	{
		printk("<1>8253 is busy");
		return -EBUSY;
	}
	Device_Open++;
	return SUCCESS;
}
static int dclose(struct inode *inode, struct file * file)
{
	Device_Open--;
	outb_p(0, 0x61);
	return SUCCESS;
}
int dioctl(struct inode * inode, struct file * file, unsigned int
	cmd, unsigned long arg)
{
	int freq_low, freq_high;
	switch (cmd)
	{
	case INIT:
	{
		outb_p(0xb6, 0x43); break;
	}
	case START:
	{

		outb_p(0x03, 0x61); break;
	}
	case FRE:
	{
		freq_low = arg & 0x00ff;
		freq_high = arg >> 8;
		outb_p(freq_low, 0x42); outb_p(freq_high, 0x42); break;
	}
	case STOP:
	{
		outb_p(0x00, 0x61); break;
	}
	default: break;
	}
	return 0;
}