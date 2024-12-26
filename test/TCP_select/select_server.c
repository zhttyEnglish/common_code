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

int main(int argc, const char * argv[])
{	
	int ret, ret1, i;
	int sock_fd, accept_fd, max_fd = 0;
	fd_set readfds, readfds_tmp;
	char buf[256];
	//check argv num
	if(3 != argc){
		printf("usage: %s <ipaddr> <port>\n", argv[0]);
		exit(-1);
	}
	
	sock_fd = socket_bind_listen(argv);//socket
	
	//init readfds & tmp
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_tmp);
	
	FD_SET(sock_fd, &readfds); //add readfds to sock_fd
	
	max_fd = (max_fd > sock_fd ? max_fd : sock_fd); //get max fd
	
	printf("waiting for client msg\n");
	while(1)
	{
		readfds_tmp = readfds;
		ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
		if(-1 == ret){
			ERRLOG("select error");
		}
		
		for(i = 0; i < max_fd + 1 && ret > 0; i++)
		{
			if(FD_ISSET(i, &readfds)){ //readfds is set
				if(i == sock_fd){
					accept_fd = accept(sock_fd, NULL, NULL); //accept
					if(-1 == accept_fd){
						ERRLOG("accept error");
					}
					printf("client [%d] connected !\n\n", accept_fd);
					FD_SET(accept_fd, &readfds);
					max_fd = (max_fd > accept_fd ? max_fd : accept_fd);
				}
				else {
					accept_fd = i;
					ret1 = recv(accept_fd, buf, sizeof(buf), 0); //recv
					if(ret1 < 0){
						perror("recv error");
						break;
					}
					else if(ret1 == 0){
						printf("client [%d] disconnected! \n", accept_fd);
						FD_CLR(accept_fd, &readfds);
						close(i);
						continue;
					}
					else{
						if(0 == strcmp(buf, "quit")){
							printf("client [%d] quit! \n", accept_fd);
							FD_CLR(accept_fd, &readfds);
							close(i);
							continue;
						}
						printf("client [%d] send data : %s\n", i, buf);
						printf("reply msg to client: ");
						scanf("%s", buf);
						ret1 = send(accept_fd, buf, sizeof(buf), 0);
						if(ret1 < 0){
							perror("send error");
							break;
						}
					}
				}
				ret --;
			}
		}
	}	
	close(sock_fd);
	return 0;
}

