/*****************************************************************************
*  TQ210 LED的demo演示                                                       *
*  Copyright (C) 2021 StephenZhou                                            *
*                                                                            *
* 这个程序是基于TQ210（三星s5pv210）架构编写                                 *
* 你可以随意修改并发布它，但是需要遵守GPL开放协议                            *
*                                                                            *
*  @file     TQ210_device_led.c                                              *
*  @brief    LED驱动                                                         *
*  Details.                                                                  *
*                                                                            *
*  @author   StephenZhou                                                     *
*  @email                                                                    *
*  @version  1.0.0.0(版本号)                                                 *
*  @date       2021-5-25                                                     *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2020/5/25 | 1.0.0.0   | StephenZhou      | LED Demo                       *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <plat/regs-serial.h>
#include <mach/regs-gpio.h>
#include <asm/uaccess.h>
#include "../../../include/TQ210_LED/device/TQ210_device_led.h"

//author
MODULE_AUTHOR("Stephen Zhou");
MODULE_LICENSE("GPL");

//led name
char LED_NAME[][256] = {{LED_ONE_NAME},{LED_TWO_NAME}};

//gpc port address
volatile unsigned long* GPIC0_address = NULL;
volatile unsigned long* GPIC0_dat     = NULL;

//drive struct
static int 	     		   led_kernel_fd 	 		 	 = 0;				//kernel struct fd
static struct class* 		   led_device_file_class 		 	 = NULL;			//sysfs fd
static struct device* 	   	   led_device_class_son[LED_NUMBER_MAX]  	 = {NULL};			//son device

//drive ope function
//open
static int led_open(struct inode* inode,struct file* file){
		
		/* open device and init */

		//get son device number
		int son_id = MINOR(inode->i_rdev);
		
		switch(son_id){
				
			case 0:	//led one
				SET_GPIC(GPIC0_address,~LED1_GPIC_BIT(0xf),&=);		//clear
				SET_GPIC(GPIC0_address,LED1_GPIC_BIT(0x1),|=);		//output
			break;

			case 1: //led two
				SET_GPIC(GPIC0_address,~LED2_GPIC_BIT(0xf),&=);		//clear
				SET_GPIC(GPIC0_address,LED2_GPIC_BIT(0x1),|=);		//output
			break;
			
			default:
				printk("TQ210_LED[ERROR]: error son device number\n");
				return -1;
			break;
				
		}

		printk("TQ210_LED[SUCCESS]:open\n");
		return 0;		
}
//wirte
static ssize_t led_write(struct file* file,const char __user* buf,size_t count,loff_t* ppos){

		//write device
		
		//get son device number
		int son_id = MINOR(file->f_dentry->d_inode->i_rdev);

		//get user variable to kernel variablei
		char val = 0;
		if(copy_from_user(&val,buf,count)){ printk("TQ210_LED[ERROR]:get user variable\n"); return -1; }
		
		switch(son_id){

			case 0:	//led one
				if(val == 1){ SET_GPIC_STATE(GPIC0_dat,LED1_GPIC_DAT_BIT(1),LED2_GPIC_DAT_BIT((GET_GPIC(GPIC0_dat,LED2_GPIC_DAT_BIT(1)))),|=); }else{ SET_GPIC_STATE(GPIC0_dat,~LED1_GPIC_DAT_BIT(1),(GET_GPIC(GPIC0_dat,0)),&=); }
				printk("TQ210_LED[MSG]:led one\n");
			break;
			
			case 1: //led two
				if(val == 1){ SET_GPIC_STATE(GPIC0_dat,LED2_GPIC_DAT_BIT(1),LED1_GPIC_DAT_BIT(GET_GPIC(GPIC0_dat,LED1_GPIC_DAT_BIT(1))),|=); }else{ SET_GPIC_STATE(GPIC0_dat,~LED2_GPIC_DAT_BIT(1),(GET_GPIC(GPIC0_dat,0)),&=); }
				printk("TQ210_LED[MSG]:led two\n");
			break;
	
			default:
				printk("TQ210[ERROR]:can't write device number\n");
			break;
		
		}
		
		printk("TQ210_LED[SUCCESS]:write\n");
		return 0;

}

//read
static ssize_t led_read(struct file* file,char __user* buf,size_t count,loff_t* ppos){
		
		//read led state
		
		int son_id = MINOR(file->f_dentry->d_inode->i_rdev);
		
		int BIT = 0;
		char on[2]  = LED_STATE_ON;
 		char off[3] = LED_STATE_OFF;
		switch(son_id){
			
			case 0:
				BIT = GET_GPIC(GPIC0_dat,LED1_GPIC_DAT_BIT(1));
				if(BIT){

					if(copy_to_user((char*)buf,&on,sizeof(on))) return -EFAULT;

				}else{
					if(copy_to_user((char*)buf,&off,sizeof(off))) return -EFAULT;
				}
			break;

			case 1:
				BIT = GET_GPIC(GPIC0_dat,LED2_GPIC_DAT_BIT(1));
				if(BIT){

					if(copy_to_user((char*)buf,&on,sizeof(on))) return -EFAULT;
					return 2;

				}else{
					if(copy_to_user((char*)buf,&off,sizeof(off))) return -EFAULT;
					return 3;
				}
			break;

			default:
				printk("TQ210[ERROR]:can't write device number\n");
				return -1;
			break;

		}

		printk("TQ210_LED[SUCCESS]:read\n");
		return 0;
}

//ioctl
static long led_ioctl(struct file* file,unsigned int cmd,unsigned long arg){

		int par = 0;
		//get son id	
		int son_id = MINOR(file->f_dentry->d_inode->i_rdev);
		
		//cmd su or err
		if(_IOC_TYPE(cmd) != MEMDEV_IOC_MAGIC)
			return -EINVAL;
		if(_IOC_NR(cmd) > MEMDEV_IOC_MAXNR)
			return -EINVAL;
		if (_IOC_DIR(cmd) & _IOC_READ){ if(access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd))){ return -EFAULT; } }
                else if(_IOC_DIR(cmd) & _IOC_WRITE) { if (access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd))){ return -EFAULT; } }
		
		switch(son_id){
		
			case 0:
				switch(cmd){

					case MEMDEV_IOCPRINT:	//Print information
						printk("TQ210_LED1 demo to stephen zhou\n");
					break;

					case MEMDEV_IOCGETDATA:	//Get parameters
						return __put_user(par,(int*) arg);
					break;	

					case MEMDEV_IOCSETDATA:	//Set parameters
						return __get_user(par,(int*) arg);
					break;

					default:
						return -EINVAL;
					break;
				}
			break;
			
			case 1:
				switch(cmd){

					case MEMDEV_IOCPRINT:	//Print information
						printk("TQ210_LED2 demo to stephen zhou\n");
					break;

					case MEMDEV_IOCGETDATA:	//Get parameters
						return __put_user(par,(int*) arg);
					break;	

					case MEMDEV_IOCSETDATA:	//Set parameters
						return __get_user(par,(int*) arg);
					break;

					default:
						return -EINVAL;
					break;
				}
			break;
		}

	        printk("TQ210_LED[SUCCESS]:ioctl\n");	
		return 0;
}

//drive struct
static struct file_operations led_drive_fops = {
	
		.owner 		= THIS_MODULE,
		.open  		= led_open,
		.write 		= led_write,
		.read  		= led_read,
		.unlocked_ioctl = led_ioctl,
};

static int __init led_init(void){

	/* register device to kernel and register to class file system and udev register to dev dir */	
 	int i = 0;	//for c99	
	//1. register to kernel struct
	led_kernel_fd = register_chrdev(0,LED_KERNEL_NAME,&led_drive_fops);
	if(led_kernel_fd < 0){
		printk("TQ210_LED[ERROR]: register_chrdev - %d\n",led_kernel_fd);
		return -1;
	}	
	
	//2. register to sysfs /sys/class dir
	led_device_file_class = class_create(THIS_MODULE,LED_SYSFS_CLASS_NAME);
	
	//3.   register son device node he will notice udev register to /dev dir
	//3.1. register son device
	for(; i < LED_NUMBER_MAX; ++i){ 
		led_device_class_son[i] = device_create (led_device_file_class,NULL,MKDEV(led_kernel_fd,i),NULL,LED_NAME[i]); 
		if(unlikely(IS_ERR(led_device_class_son[i]))) { 
			printk("TQ210_LED[ERROR]: register son device\n");
			return -2;
		}
	}
	
	//4. map gpio address to virtual address
	GPIC0_address = (volatile unsigned long*)ioremap(GPIC0_CON_ADDRESS,GPIC_ADDRESS_FORMAT);
	GPIC0_dat     = (volatile unsigned long*)ioremap(GPIC0_DAT_ADDRESS,GPIC_ADDRESS_FORMAT);	
	
	printk("TQ210_LED[SUCCESS]:init\n");
	return 0;
}

static void __exit led_exit(void){

	//delete device
	int i = 0;	//for c99
	
	//class son
	for(; i < LED_NUMBER_MAX; ++i){
		device_unregister(led_device_class_son[i]);
	}

	//class
	class_destroy(led_device_file_class);
	
	//io map
	iounmap(GPIC0_address);
	iounmap(GPIC0_dat);
	
	//kerbel struct
	unregister_chrdev(led_kernel_fd,LED_KERNEL_NAME);
	
	printk("TQ210_LED[SUCCESS]:exit\n");
}

module_init(led_init);
module_exit(led_exit);
