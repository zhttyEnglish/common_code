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
#define DEFAULT_IP "192.168.2.219"

int sockfd = -1;

int socket_connect()
{
	struct sockaddr_in server_addr;

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1){
		log("socket create fail : %s\r\n",strerror(errno));
		return -1;
	}
	log("socket create success\r\n");
	
    	memset(&server_addr, 0, sizeof(server_addr));
    	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(DEFAULT_PORT);
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, DEFAULT_IP, &server_addr.sin_addr);

	if(connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		log("connect error for %s\r\n",strerror(errno));
		return -1;
	}
	return fd;
}

int send_file()
{
	int total_send_bytes = 0;
	int ready_to_send_bytes = 0;
	int read_bytes = -1;
	int ret = -1;

	char * buf = (char *)malloc(sizeof(char) * 1024000);

	FILE * fp = fopen("./test.jpg", "r");

	while((read_bytes = fread(buf, 1, 1024000, fp)) > 0)
	{
		total_send_bytes = 0;
		ready_to_send_bytes = read_bytes;

		while(ready_to_send_bytes != 0)
		{
			ret = send(sockfd, &buf[total_send_bytes], ready_to_send_bytes, 0);
			if(ret == ready_to_send_bytes){
				log("----- ready_to_send = %d ret = %d\r\n", ready_to_send_bytes, ret);
				break;			
			}
			else if(ret > 0){
				total_send_bytes += ret;
				ready_to_send_bytes -= ret;
				log("ret = %d total send = %d ready to send = %d\r\n", ret, total_send_bytes, ready_to_send_bytes);
				continue;
			}else if(ret < 0 && errno == 11){
				log("----- send ret < 0 errno = %d %s\r\n", errno, strerror(errno));
				continue;
			}
		}

		log("total send = %d ready_to_send = %d\r\n", total_send_bytes, ready_to_send_bytes);
	}
	log("send file done\r\n");

	fclose(fp);
	free(buf);
	return 0;
}

int main()
{
	int ret = -1;
	
	sockfd = socket_connect();

	ret = send_file();
	if(ret < 0){
		log("----- send file error -----\r\n");		
	}

	return 0;
}


