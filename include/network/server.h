#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "../../include/TQ210_LED/device/TQ210_device_led.h"

//server config
#define PORT 			8081
#define BACKLOG  		10

//http
#define STATE_OK 		"HTTP/1.0 200 OK\r\n"
#define SERVER_TYPE     	"Server: DWBServer\r\nContent-Type: text/html;charset=utf-8\r\n\r\n"

//file
#define INDEX_NAME 		"./index.html"

//http ops
#define STATE_GET_INDEX  	1
#define STATE_BUTTON_FOO 	2
#define STATE_200	 	3

//led
#define LED_1			1
#define LED_2			2
#define LED_UP			3
#define LED_DOWN		4
