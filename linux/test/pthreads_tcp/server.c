#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define ERRLOG(errmsg)											\
	do 															\
	{															\
		printf("%s -- %s : (%d)", __FILE__, __func__, __LINE__);\
		perror(errmsg);											\
		exit(-1);												\
	} while(0)

typedef struct{
	int accept_fd;
	struct sockaddr_in client_addr;
}num_t;

int socket_bind_listen(const char * argv[])
{
	int sock_fd, ret;
	struct sockaddr_in server_addr;
	
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
	
	//bind
	ret = bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(-1 == ret){
		ERRLOG("bind error");
	}
	
	//listen
	ret = listen(sock_fd, 5);
	if(-1 == ret){
		ERRLOG("listen error");
	}
	
	return sock_fd;
}

void * task1(void * arg)
{
	num_t usr;
	int ret;
	char buf[256];
	
	usr = *(num_t *)arg;
	while(1){
		ret = recv(usr.accept_fd, buf, sizeof(buf), 0);
		if(ret < 0){
			ERRLOG("recv error");
		}
		else if(ret == 0){
			printf("client %s : %d disconnected!\n", 
				inet_ntoa(usr.client_addr.sin_addr), ntohs(usr.client_addr.sin_port));
			break;
		}
		else{
			if(strcmp(buf, "quit") == 0){
				printf("client %s : %d quit!\n", 
					inet_ntoa(usr.client_addr.sin_addr), ntohs(usr.client_addr.sin_port));
				break;
			}else{
				printf("client %s : %d send msg : %s\n", 
					inet_ntoa(usr.client_addr.sin_addr), ntohs(usr.client_addr.sin_port), buf);
				printf("reply msg to client : ");
				gets(buf);
				ret = send(usr.accept_fd, buf, sizeof(buf), 0);
				if(ret < 0){
					ERRLOG("send error");
				}
			}
		}
	}
	close(usr.accept_fd);
	pthread_exit(NULL);
}

int main(int argc, const char * argv[])
{
	int sock_fd, ret;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	pthread_t pid;
	num_t usr;
	
	//check argv num
	if(3 != argc){
		printf("usage: %s <ipaddr> <port>\n", argv[0]);
		exit(-1);
	}
	
	sock_fd = socket_bind_listen(argv);//socket
	client_addr_len = sizeof(client_addr);
	memset(&client_addr, 0, client_addr_len);
	memset(&usr, 0, sizeof(usr));
	
	while(1){
		usr.accept_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if(-1 == usr.accept_fd){
			ERRLOG("accrpt error");
		}else{
			printf("client %s : %d connected!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		}
		
		usr.client_addr = client_addr;
		ret = pthread_create(&pid, NULL, task1, (void *)&usr);
		if(ret){
			ERRLOG("pthread create error");
		}
		pthread_detach(pid);
	}
	close(sock_fd);
	return 0;
}

