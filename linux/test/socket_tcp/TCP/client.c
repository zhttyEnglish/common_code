#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
#define MAXLIN 4096

int main(int argc, char ** argv)
{
	int socket_fd, recvive_len;
	char buf[MAXLIN], send_buf[MAXLIN];
	struct sockaddr_in server_addr;
	int ret;
	
	if(argc!=2)
    {
        printf("usage: ./client <ipaddress>\n");
        return -1;
    }
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		printf("socket create fail : %s\n",strerror(errno));
		return -1;
	}
	printf("socket create success\n");
	
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
	server_addr.sin_port = htons(DEFAULT_PORT);
	
	ret = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	if(ret <= 0){
		printf("inet_pton error for %s\n",argv[1]);
		return -1;
	}
	
	ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("connect error for %s\n",strerror(errno));
		return -1;
	}
	
	printf("send msg to server: \n");
	fgets(send_buf, 4096, stdin);
	if(!fork()){
		ret = send(socket_fd, send_buf, strlen(send_buf), 0);
        if(ret < 0){
            printf("send msg error :%s\n",strerror(errno));
            return -1;
        }
    }
    if((recvive_len = recv(socket_fd, buf, MAXLIN, 0)) == -1)
    {
        printf("recv error\n");
        return -1;
    }
    buf[recvive_len]='\0';
    printf("Received : %s\n",buf);
    close(socket_fd);
    return 0;
}

