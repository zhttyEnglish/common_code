#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define port 8000

/* socket_bind_listen	创建socket,绑定,监听
 * return 	返回 	sockfd 
 */
int socket_bind_listen()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		printf("socket error\r\n");
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  
    	addr.sin_family = AF_INET;  
    	addr.sin_port = htons(port);  
    	addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	printf("port %d\r\n", port);
	
	int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0){
		printf("bind error\r\n");
		return -2;
	}

	ret = listen(sockfd, 10);
	if(ret < 0){
		printf("listen error\r\n");
		return -3;
	}

	return sockfd;
}

int main()
{
	char buf[1024] = {0};

	int ret = -1, len = -1, sockfd = -1, acceptfd = -1;
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	
	printf("start http server\r\n");

	fd_set rset, allset; 
	int client[10] = {0};
	int client_port[10] = {0};
	char client_ip[10][16] = {0};
	int maxfd = -1, maxi = -1, nready, i, tmpfd;
	while(1)
	{
		sockfd = socket_bind_listen();
		if(sockfd < 0){
			printf("socket_bind_listen error\r\n");
			break;
		}
		printf("socket_bind_listen success %d\r\n", sockfd);
		
		maxfd = sockfd;
		FD_ZERO(&allset);
		FD_SET(sockfd, &allset);   

		while(1)
		{
			rset = allset;
			struct timeval timeout;
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			nready = select(maxfd + 1, &rset, NULL, NULL, &timeout);
			if (nready < 0){
				printf("select error\r\n");
				break;
			}
			if (FD_ISSET(sockfd, &rset)) 
			{
				acceptfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
				if(acceptfd < 0){
					printf("accept error\r\n");
					break;
				}
				struct sockaddr_in * sock = (struct sockaddr *)&client_addr;
				char temp_ip[16] = {0};
				int temp_port = ntohs(sock->sin_port);
				struct in_addr in = sock->sin_addr;
				inet_ntop(AF_INET, &in, temp_ip, 16);
				
				printf("ip:port %s:%d client[%d] accept success\r\n", temp_ip, temp_port, acceptfd);

				for (i = 0; i < 10; i++){
					if (client[i] <= 0) {                           
						client[i] = acceptfd;  
						client_port[i] = temp_port;
						memcpy(client_ip[i], temp_ip, 16);
						break;
					}
				}
				if (i == 10) {                              
					printf("too many clients\r\n");
					break;
				}
				
				FD_SET(acceptfd, &allset);
				
				if (acceptfd > maxfd){
					maxfd = acceptfd;                      
				}
				if (i > maxi){
					maxi = i;
				}
				if (--nready == 0){
					continue;
				}
			}

			for (i = 0; i <= maxi; i++) 
			{                              
				if ((tmpfd = client[i]) < 0) continue;
				
				if (FD_ISSET(tmpfd, &rset)) 
				{
					memset(buf, 0, 1024);
					ret = recv(tmpfd, buf, 1024, 0);
					if(ret < 0){
						printf("receive error\r\n");
						return -1;
					}
					else if(ret == 0){
						printf("ip:port %s:%d client[%d] disconnected\r\n", client_ip[i], client_port[i], tmpfd);
						close(tmpfd);
						client[i] = -1;
						client_port[i] = 0;
						memset(client_ip[i], 0, 16);
						FD_CLR(tmpfd, &allset);
					}
					else if(ret > 0)
					{					
						printf("ip:port %s:%d client[%d] send :\r\n%s\r\n", client_ip[i], client_port[i], tmpfd, buf);
						
						ret = send(tmpfd, buf, strlen(buf), 0);
						if(ret < 0){
							printf("build_error_callback  send error\r\n");
							break;
						}
					}
					
					if (--nready == 0) break; 
				}
			}
		}
		close(sockfd);
	}
	return 0;
}



