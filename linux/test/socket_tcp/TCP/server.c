#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_PORT 8000
#define MAXLIN 4096

//socket server 
int main(int argc, char ** argv)
{
	int socket_fd, connect_fd;
	struct sockaddr_in server_addr;
	char buf[4096];
	int num, ret;
	
	//创建socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		printf("socket create fail : %s\n",strerror(errno));
		return -1;
	}
	printf("socket create success\n");
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(DEFAULT_PORT);
	
	//绑定
	ret = bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret == -1){
		printf("bind fail : %s\n", strerror(errno));
		return -1;
	}
	
	//监听
	ret = listen(socket_fd, 10);
	if(ret == -1){
		printf("listen fail : %s\n", strerror(errno));
		return -1;
	}
	
	printf("**********waiting for client request**********\n");
	
	//接收，发送
	while(1)
	{
		connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL);
		if(connect_fd == -1){
			printf("accept fail : %s\n", strerror(errno));
			continue;
		}
		
		num = recv(connect_fd, buf, MAXLIN, 0);
		if(!fork()){
			if(send(connect_fd, "_seosootang", 26, 0) == -1){
				printf("send fail \n");
				close(connect_fd);
				return -1;
			}
		}
		
		buf[num] = '\0';
		printf("receive client : %s\n", buf);
		close(connect_fd);		
	}
	close(socket_fd);
	return 0;
}
