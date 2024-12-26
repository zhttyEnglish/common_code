int tcp_client()
{   
	char buf[300] = {0};
	while(1){
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1){
            tcplog("socket error\r\n");
            return -1;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr("49.234.245.231");
        server_addr.sin_port = htons(8301);

        int confd = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

        while(confd > 0){
            int ret = send(sockfd, buf, len, 0);
            if(ret < 0){
                tcplog("send error\r\n");
                return -1;
            }
        
            ret = recv(sockfd, buf, 300, 0);
            if(ret <= 0){
                tcplog("recv error\r\n");
                return -1;
            }
        }
        close(sockfd);
    }
    return 0;
}


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main(){
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("192.168.183.138");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
 
    listen(serv_sock, 20);
    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    //向客户端发送数据
    char str[] = "Hello World!";
    write(clnt_sock, str, sizeof(str));
 
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
