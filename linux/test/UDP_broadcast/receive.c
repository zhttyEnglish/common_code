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

int socket_bind(const char * argv[])
{
	int sock_fd, ret;
	struct sockaddr_in server_addr;
	
	// create socket
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == sock_fd){
		ERRLOG("socket error");
	}
	
	//set server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	//bind
	ret = bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(-1 == ret){
		ERRLOG("bind error");
	}
	return sock_fd;
}

int main(int argc, const char * argv[])
{	
	int sock_fd, ret;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	char buf[256];
	
	if(3 != argc){
		printf("usage: %s <ipaddr> <port>\n", argv[0]);
		exit(-1);
	}
	
	sock_fd = socket_bind(argv);
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr_len = sizeof(client_addr);
	while(1){
		ret = recvfrom(sock_fd, buf, sizeof(buf), 0, 
			(struct sockaddr *)&client_addr, &client_addr_len);
		if(ret < 0){
			ERRLOG("recvfrom error");
		}
		printf("client[%s:%d] send msg: %s\n", 
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
		
	}
	close(sock_fd);
	return 0;
}

