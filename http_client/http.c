#include <stdio.h>
#include <string.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "http.h"

#define HTTP_IP  "192.168.3.131"
#define HTTP_HOST 8081
#define DOMIN	"www.baidu.com"

int build_http_post_packet(char * buf, char * filename, char * content)
{
	int pos = 0;

	pos += sprintf(&buf[pos], "POST %s HTTP/1.1\n", filename);
	pos += sprintf(&buf[pos], "HOST: %s\n", HTTP_IP);
	pos += sprintf(&buf[pos], "Content-Type: %s\n", "application/json");
	pos += sprintf(&buf[pos], "Content-Length: %d\n", strlen(content));
	pos += sprintf(&buf[pos], "\n");
	pos += sprintf(&buf[pos], "%s\r\n", content);
	pos += sprintf(&buf[pos], "\r\n");

	return pos;
}

/*  tcp_connect   创建tcp连接
 *  
 *	return   返回sock句柄
 */
int tcp_connect()
{
//	struct hostent *l_hostent;
//	l_hostent = gethostbyname(DOMIN);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("socket error\r\n");
		return -1;
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(HTTP_IP);
//	memcpy(&server_addr.sin_addr.s_addr, *l_hostent->h_addr_list, strlen(*l_hostent->h_addr_list));
	server_addr.sin_port = htons(HTTP_HOST);

	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("connect error\r\n");
		return -2;
	}
	return sockfd;
}

int socket_bind_listen()
{
	int opts = -1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		printf("socket error\r\n");
		return -1;
	}

//	opts = fcntl(sockfd, F_GETFL, 0);
//	if(opts < 0){
//		printf(">>>>>> F_GETFL error 1\r\n");
//		return -1;
//	}
//	opts = opts | O_NONBLOCK;
//	if(fcntl(sockfd, F_SETFL, opts) < 0){
//		printf(">>>>>> F_SETFL error 2 \r\n");
//		return -1;
//	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(HTTP_HOST);  
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	printf("port %d\r\n", HTTP_HOST);
//	int opt = 1;
//    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)//闃叉鍦板潃鍗犵敤
//    {
//        printf("Server setsockopt failed\r\n");
//        return -4;
//    }
	
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

int main(int argc, char *argv[])
{
	char buf[1024] = {0};
	int len = -1;
//	char * content_B = "{\"requestId\":\"qwertyuiop\",\"objectList\":[{\"taskId\":\"12345\",\"taskInsId\":\"12345678\",\"taskNodeId\":\"1234567890\",\"beuId\":\"12qwe\",\"beuName\":\"asadasd\",\"type\":[{\"aaa\",\"bbb\",\"ccc\"}],\"algName\":\"xxxxx\",\"subtypeList\":[{\"zzz\",\"xxx\",\"ccc\"}],\"imageUrlList\":[{\"qqqq\",\"wwww\",\"eeee\"}],\"imageNormalUrlPath\":\"rrrrrr\",\"actionType\":\"1\",\"mspId\":\"111\",\"users\":\"asdadfva\",\"infraredArea\":[{\"x\":\"4096\",\"y\":\"4880\"},{\"x\":\"5542\",\"y\":\"4880\"},{\"x\":\"5542\",\"y\":\"1728\"},{\"x\":\"4096\",\"y\":\"1728\"}]}]}";
//	char * content_A = "{\"requestHostIp\":\"qqqqqqqq\",\"requestHostPort\":\"wwwwwwww\",\"requestId\":\"eeeeeeee\",\"objectList\":[{\"objectId\":\"121\",\"typeList\":[\"bj_bpmh\",\"bj_bpps\"],\"imageNormalPath\":[\"qwerty\"],\"imageList\":[\"jpg.jpeg\"]}]}";
//	int sockfd = tcp_connect();
	int sockfd = socket_bind_listen();
	if(sockfd < 0){
		printf("tcp  connect error\r\n");
		return -1;
	}
	printf("tcp_connect success\r\n");
	
//	len = build_http_post_packet(buf, "/http/http_client/http", content_B);
	
//	printf("send : %s\r\n", buf);
	
//	int ret = send(sockfd, buf, len, 0);
//	if(ret < 0){
//		printf("send error\r\n");
//		return -1;
//	}
//	memset(buf, 0, 1024);
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	int acceptfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

	int ret = recv(acceptfd, buf, 1024, 0);
	if(ret <= 0){
		printf("recv error\r\n");
		return -1;
	}
	printf("http receive demo recv : \r\n%s\r\n", buf);
	close(acceptfd);
	close(sockfd);
	return 0;
}

