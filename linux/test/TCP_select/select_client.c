#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
#define ERRLOG(errmsg)											\
	do 															\
	{															\
		printf("%s -- %s : (%d)", __FILE__, __func__, __LINE__);\
		perror(errmsg);											\
		exit(-1);												\
	} while(0)
	
int main(int argc, const char * argv[])
{	
	int sock_fd, ret;
	char buf[256];
	struct sockaddr_in server_addr;
	
	//check argv num
	if(3 != argc){
		printf("usage: %s <ipaddr> <port>\n", argv[0]);
		exit(-1);
	}
	// create socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sock_fd){
		ERRLOG("socket error");
	}
	
	//set server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	//connect
	ret = connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(-1 == ret){
		ERRLOG("connect error");
	}
	else{
		printf("connect to server success\n\n");
	}
	
	while(1)
	{
		printf("send msg to server: ");
		gets(buf);
		//send
		ret = send(sock_fd, buf, sizeof(buf), 0);
		if(ret < 0){
			ERRLOG("send error");
		}
		
		if(0 == strcmp(buf, "quit"))
			break;
		//recv
		ret = recv(sock_fd, buf, sizeof(buf), 0);
		if(ret < 0){
			ERRLOG("recv error");
		}
		printf("receive data from server : %s\n", buf);
	}
	close(sock_fd);
	return 0;
}
