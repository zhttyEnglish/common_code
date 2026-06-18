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

#define DEFAULT_PORT 8000 
#define DEFAULT_IP "192.168.2.219"

int sockfd = -1;
int acceptfd = -1;

int socket_bind_listen()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1){
		log("socket create fail : %s\r\n",strerror(errno));
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  
    	addr.sin_family = AF_INET;  
    	addr.sin_port = htons(DEFAULT_PORT);  
    	//addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	inet_pton(AF_INET, DEFAULT_IP, &addr.sin_addr);
	

	int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0){
		log("bind error : %s\r\n", strerror(errno));
		return -1;
	}

	ret = listen(fd, 10);
	if(ret < 0){
		log("listen error : %s\r\n", strerror(errno));
		return -1;
	}
	log("----- done -----\r\n");
	return fd;
}

int recv_file()
{
	int recv_bytes = 0;	
	int ret = 0;
	char * buf = (char *)malloc(sizeof(char) * 1024000);

	FILE * fp = fopen("recv.jpg", "w");

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	acceptfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
	if(acceptfd < 0){
		log("accept error %s\r\n", strerror(errno));
		return -1;
	}
	log("accept success\r\n");

	while(sockfd != -1)
	{
		recv_bytes = recv(acceptfd, buf, 1024000, 0);
		if(recv_bytes > 0)
		{
			ret = fwrite(buf, sizeof(char), recv_bytes, fp);
			log("recv_bytes = %d, ret = %d\r\n", recv_bytes, ret);
		}
		else if(recv_bytes == 0){
			log("client discontect\r\n");
			sockfd = -1;	
		}
	}
	log("recv file done\r\n");
	
	fclose(fp);
	free(buf);
	
	return 0;
}

int main()
{
	sockfd = socket_bind_listen();

	recv_file();

	return 0;
}


