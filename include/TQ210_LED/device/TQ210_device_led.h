/*opyright (C) 2021 StephenZhou                                              *
*                                                                            *
* 这个程序是基于TQ210（三星s5pv210）架构编写                                 *
* 你可以随意修改并发布它，但是需要遵守GPL开放协议                            *
*                                                                            *
*  @file     TQ210_device_led.h                                              *
*  @brief    LED head                                                        *
*  Details.                                                                  *
*                                                                            *
*  @author   StephenZhou                                                     *
*  @email                                                                    *
*  @version  1.0.0.0(版本号)                                                 *
*  @date       2021-5-27                                                     *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*/

//led one and two name
#define LED_ONE_NAME "TQ210_LED_ONE"
#define LED_TWO_NAME "TQ210_LED_TWO"

//led name in kernel
#define LED_KERNEL_NAME         "TQ210_LED"
#define LED_SYSFS_CLASS_NAME    "TQ210_LED_CLASS"

//led device number max
#define LED_NUMBER_MAX 2

//set or get gpic bit value
#define SET_GPIC(GPIC_ADDRESS,VALUE,OPE)                                *GPIC_ADDRESS OPE VALUE
#define SET_GPIC_STATE(GPIC_ADDRESS,LED1_VALUE,LED2_VALUE,OPE)          *GPIC_ADDRESS OPE (LED1_VALUE | LED2_VALUE)
#define GET_GPIC(GPIC_ADDRESS,VALUE)                                    *GPIC_ADDRESS & VALUE

//led gpic port
#define LED1_GPIC_BIT(VALUE) (VALUE << 12)
#define LED2_GPIC_BIT(VALUE) (VALUE << 16)
#define LED1_GPIC_DAT_BIT(VALUE) (VALUE << 3)
#define LED2_GPIC_DAT_BIT(VALUE) (VALUE << 4)

//device gpic address
#define GPIC0_CON_ADDRESS 0xE0200060
#define GPIC0_DAT_ADDRESS 0xE0200064
#define GPIC_ADDRESS_FORMAT 16

//state
#define LED_STATE_ON  "ON"
#define LED_STATE_OFF "OFF"

//ioctl
#define MEMDEV_IOC_MAGIC  's'
#define MEMDEV_IOCPRINT   _IO(MEMDEV_IOC_MAGIC, 1)
#define MEMDEV_IOCGETDATA _IOR(MEMDEV_IOC_MAGIC, 2, int)
#define MEMDEV_IOCSETDATA _IOW(MEMDEV_IOC_MAGIC, 3, int)
#define MEMDEV_IOC_MAXNR  3

//LED NAME
#define LED_DEV_ONE_NAME "/dev/TQ210_LED_ONE"
#define LED_DEV_TWO_NAME "/dev/TQ210_LED_TWO"
