#include "../../include/network/server.h"
char index_body[2*1024] = {0};
int GetHead(char* buff,char* t){
	
	if(buff == NULL || t == NULL){
		return -1;
	}
	int str_len = strlen(buff);
	int i = 0;
	for(; i<str_len ;++i){

		if(buff[i] == '\r'){
			break;
		}

		t[i] = buff[i];
		
	}

	return 0;
	
}

int GetEnd(char* buff,char* t){
	if(buff == NULL || t == NULL){
		return -1;
	}

	int i = strlen(buff);
	int count = 0;
	int d = 0;
	int y = 0;
	for(;i>0;--i){
		if(buff[i] == '\n'){
			break;
		}
		++count;
	}
	
	d = strlen(buff) - count+1;
	for(;buff[d] != '\0';++d){
		t[y++] = buff[d]; 
		
	}
	
	return 0;
}

int GET_HTTP_STATE(char* buff){
	
	if(buff == NULL){
		return 0;
	}
	
	char Head[256] = {0};
	GetHead(buff,Head);
	if(Head == NULL){ return 0; }
	
	if(strcmp(Head,"GET / HTTP/1.1") == 0){
		
		return STATE_GET_INDEX;

	}

	char ff[256] = {0};
	if(strcmp(Head,"POST / HTTP/1.1") == 0){
				
		return STATE_BUTTON_FOO;

	}

	return STATE_200;

}

int Led_Ops(int LED_INDEX,int STATE){
	
	int fd  = 0;
	int val = 0;

	if(LED_INDEX == LED_1){
		
		fd = open(LED_DEV_ONE_NAME,O_RDWR);
		if(fd == -1) { return -1; }
	}
	
	if(LED_INDEX == LED_2){
		fd = open(LED_DEV_TWO_NAME,O_RDWR);
		if(fd == -1) { return -1; }
	}
	
	if(STATE == LED_UP){
		val = 1;
		write(fd,&val,sizeof(val));
	}

	if(STATE == LED_DOWN){
		val = 0;
		write(fd,&val,sizeof(val));
	}

	close(fd);

}

int Http_Exec(int state,int fd,char* buff){
	
	char rt[2*1024] = {0};
	if(state == STATE_GET_INDEX){
		strcat(rt,STATE_OK);
		strcat(rt,SERVER_TYPE);
		strcat(rt,index_body);
		send(fd,rt,strlen(rt),0);
	}

	if(state == STATE_BUTTON_FOO){
		char fun[256] = {0};
		GetEnd(buff,fun);
		strcat(rt,STATE_OK);
		strcat(rt,SERVER_TYPE);
		strcat(rt,index_body);
		send(fd,rt,strlen(rt),0);
		if(strcmp(fun,"foo=LED1_Light") == 0){
			Led_Ops(LED_1,LED_UP);
		}
		if(strcmp(fun,"foo=LED1_Ext") == 0){
			Led_Ops(LED_1,LED_DOWN);
		}
		if(strcmp(fun,"foo=LED2_Light") == 0){
			Led_Ops(LED_2,LED_UP);
		}
		if(strcmp(fun,"foo=LED2_Ext") == 0){
			Led_Ops(LED_2,LED_DOWN);
		}
		
	}

	if(state == STATE_200){
		strcat(rt,STATE_OK);
		strcat(rt,SERVER_TYPE);
		send(fd,rt,strlen(rt),0);	
	}
	
	return 0;
}

int main(){

	int listenfd, connectfd;
	struct sockaddr_in server, client;
    	socklen_t addrlen;
    
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        	perror("[SERVER_ERROR] socket\n");
    		return -1;
	}

	int opt = 1;
    	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    	bzero(&server, sizeof(server));
    	server.sin_family = AF_INET;
    	server.sin_port = htons(PORT);
    	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
   	if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1){
        	perror("[SERVER_ERROR] bind\n");
    		return -1;
	}

	addrlen = sizeof(client);
	
	FILE* fp = fopen(INDEX_NAME,"r");
	if(fp == NULL){
		perror("[SERVER_ERROR] index.html file\n");
		return -1;
	}	

	fread(index_body,sizeof(index_body),1,fp);
	
    	while(1){

    		if(listen(listenfd, BACKLOG) == -1){
        		perror("[SERVER_ERROR] listen\n");
			return -1;
        	}

		if((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1){
        		perror("[SERVER_ERROR] accept\n");
			return -1;
		}
		
		char buff[1024] = {0};
	        recv(connectfd,buff,1024,0);
		//printf("%s\n",buff);
        	Http_Exec(GET_HTTP_STATE(buff),connectfd,buff);	
		close(connectfd);
    	}

    	close(listenfd);
    	return 0;

}
