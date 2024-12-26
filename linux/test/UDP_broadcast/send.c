#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ERRLOG(errmsg)											\
	do 															\
	{															\
		printf("%s -- %s : (%d)", __FILE__, __func__, __LINE__);\
		perror(errmsg);											\
		exit(-1);												\
	} while(0)
	
int main(int argc, char * argv[])
{
	int sock_fd, ret;
	int on_off = 1;
	struct sockaddr_in client_addr;
	char buf[256];

	if(3 != argc){
		printf("usage: %s <ipaddr> <port>\n", argv[0]);
		exit(-1);
	}
	//create socked
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == sock_fd){
		ERRLOG("socket error");
	}
	
	//set server address
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(argv[1]);
	client_addr.sin_port = htons(atoi(argv[2]));

	ret = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &on_off, sizeof(on_off));
	if(ret < 0){
		ERRLOG("setsockopt error");
	}
	
	while(1){
		printf("input: ");
		gets(buf);
		
		if(strcmp(buf, "quit") == 0){
			break;
		}
		
		ret = sendto(sock_fd, buf, sizeof(buf), 0, 
				(struct sockaddr *)&client_addr, sizeof(client_addr));
		if(ret < 0){
			ERRLOG("sendto error");
		}
	}
	close(sock_fd);
	return 0;
}
