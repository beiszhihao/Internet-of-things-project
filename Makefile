ifneq ($(KERNELRELEASE),)
	obj-m  := ./src/TQ210_LED/device/TQ210_device_led.o
else
        KDIR := /home/beis/TQ/opt/EmbedSky/TQ210/Kernel_3.0.8_TQ210_for_Linux_v2.4
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-embedsky-linux-gnueabi-
	arm-embedsky-linux-gnueabi-gcc ./src/TQ210_LED/app/TQ210_app_led.c -o TQ210_app_led
	arm-embedsky-linux-gnueabi-gcc ./src/network/server.c -o server
	mv ./src/TQ210_LED/device/*.o 			./output
	mv *.symvers 					./output
	mv *.order 					./output
	mv ./src/TQ210_LED/device/*.mod.c     		./output
	mv ./src/TQ210_LED/device/*.ko		 	./arch/arm/TQ210_LED
	mv TQ210_app_led 				./arch/arm/TQ210_LED
	mv server		 			./arch/arm/network
	cp ./src/network/index.html			./arch/arm/network
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm ./output/*
	rm ./arch/arm/TQ210_LED/*
	rm ./arch/arm/network/*
endif
 
