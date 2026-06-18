#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DEBUG_OPEN 1

#if DEBUG_OPEN
#define log(format, ...) \
  			printf("[%s:%d] "format, __func__, __LINE__,  ##__VA_ARGS__)
#else
#define log(format, ...)
#endif

/*********************** client ********************/

#define DEFAULT_PORT 8000 
#define DEFAULT_IP "192.168.0.123"

int sockfd = -1;

int tcp_connect(char * request_ip, int request_post)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("socket error\r\n");
		return -1;
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(request_ip);
	server_addr.sin_port = htons(request_post);
	printf("IP  port  %s : %d\r\n", request_ip, request_post);
	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("connect error\r\n");
		return -2;
	}
	return sockfd;
}

int main()
{
	int ret = -1;
	char buf[32] ={0};
	sockfd = tcp_connect("192.168.0.123", 8000);

	ret = send(sockfd, "abcdefg", 7, 0);
	if(ret < 0){
		log("send error : %s\r\n", strerror(errno));
	}
	ret = recv(sockfd, buf, 32, 0);
	if(ret > 0){
		log("recv ret = %d %s\r\n", ret, buf);		
	}

	while(1);
	close(sockfd);
	return 0;
}


