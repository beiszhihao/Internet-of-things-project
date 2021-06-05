/*****************************************************************************
*  TQ210 LED的demo演示                                                       *
*  Copyright (C) 2021 StephenZhou                                            *
*                                                                            *
* 这个程序是基于TQ210（三星s5pv210）架构编写                                 *
* 你可以随意修改并发布它，但是需要遵守GPL开放协议                            *
*                                                                            *
*  @file     TQ210_device_led.c                                              *
*  @brief    LED User                                                        *
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

//head file
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "../../../include/TQ210_LED/device/TQ210_device_led.h"

void print(char* led1_state,char* led2_state){
	system("clear");
	printf("/*****************************************************************************\n");
	printf("*									     *\n");	
	printf("*  TQ210 LED的demo演示                                                       *\n");
	printf("*  Copyright (C) 2021 StephenZhou                                            *\n");
	printf("*									     *\n");
	printf("*----------------------------------------------------------------------------*\n");
	printf("*  Device         : State                                                    *\n");
	printf("*----------------------------------------------------------------------------*\n");
	if(strcmp(led1_state,"ON") == 0){ printf("*  TQ210_LED_ONE  : ON							     *\n"); }
	else{ printf("*  TQ210_LED_ONE  : OFF							     *\n"); }
	printf("*----------------------------------------------------------------------------*\n");
	if(strcmp(led2_state,"ON") == 0){ printf("*  TQ210_LED_TWO  : ON							     *\n"); }
	else{ printf("*  TQ210_LED_TWO  : OFF							     *\n"); }
	printf("*----------------------------------------------------------------------------*\n");
	printf("*  Change History :                                                          *\n");
	printf("*  <Date>     | <Version> | <Author>       | <Description>                   *\n");
	printf("*----------------------------------------------------------------------------*\n");
	printf("*  2020/5/25 | 1.0.0.0   | StephenZhou      | LED Demo                       *\n");
	printf("*----------------------------------------------------------------------------*\n");
	printf("*                                                                            *\n");
	printf("*****************************************************************************/\n");

}
int main(int argc,char **argv){
	
	/* Two LEDs flash each other and print the status */
	int cmd = 0,arg = 0,val = 0;
	char led1_state[4] = {0},led2_state[4] = {0};
	//1. open udev device
	int led_fd1 = open(LED_DEV_ONE_NAME,O_RDWR);
	int led_fd2 = open(LED_DEV_TWO_NAME,O_RDWR);
	if(led_fd1 == -1 || led_fd2 == -1){
		printf("error:can't open led device\n");
		return -1;
	}

	//2. print msg to kernel ioctl
	cmd = MEMDEV_IOCPRINT;
	if(ioctl(led_fd1,cmd,&arg) == -1) { printf("error:can't ioctl for led1\n"); return -1; }
	cmd = MEMDEV_IOCPRINT;
	if(ioctl(led_fd2,cmd,&arg) == -1) { printf("error:can't ioctl for led2\n"); return -1; }
        
	//3. wink
	while(1){
		
		//led one wink led two close
		val = 1;
		if(write(led_fd1,&val,sizeof(val)) == -1){ printf("error:can't write for led1\n"); return -1; }		
		val = 0;
		if(write(led_fd2,&val,sizeof(val)) == -1){ printf("error:can't write for led2\n"); return -1; }		
		
		//clear string
		memset(led1_state,0,sizeof(led1_state));
		memset(led2_state,0,sizeof(led2_state));
		
		//read led state
		if(read(led_fd1,led1_state,sizeof(led1_state)) == -1){ printf("error:cant't read for led1\n"); return -1; }			
		if(read(led_fd2,led2_state,sizeof(led2_state)) == -1){ printf("error:cant't read for led2\n"); return -1; }			
		print(led1_state,led2_state);
	
		sleep(3);

		//led two wink led one close
		val = 1;
		if(write(led_fd2,&val,sizeof(val)) == -1){ printf("error:can't write for led1\n"); return -1; }		
		val = 0;
		if(write(led_fd1,&val,sizeof(val)) == -1){ printf("error:can't write for led2\n"); return -1; }	
		
		//clear string
		memset(led1_state,0,sizeof(led1_state));
		memset(led2_state,0,sizeof(led2_state));
		
		//read led state
		if(read(led_fd1,led1_state,sizeof(led1_state)) == -1){ printf("error:cant't read for led1\n"); return -1; }			
		if(read(led_fd2,led2_state,sizeof(led2_state)) == -1){ printf("error:cant't read for led2\n"); return -1; }			
		print(led1_state,led2_state);
		
		sleep(3);
		
	}

	return 0;
}
