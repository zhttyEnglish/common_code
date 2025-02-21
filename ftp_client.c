#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")  // 链接Winsock库

int connect_to_ftp_server(const char *server_ip) {
    WSADATA wsaData;
    SOCKET controlSocket;
    struct sockaddr_in serverAddr;

    // 初始化WinSock库
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return -1;
    }

    // 创建TCP套接字
    controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (controlSocket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return -1;
    }

    // 设置服务器地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(21);  // FTP控制端口

    // 使用inet_pton将IP地址转换为二进制格式
    if (inet_pton(AF_INET, server_ip, &serverAddr.sin_addr) <= 0) {
        printf("Invalid address or address not supported.\n");
        closesocket(controlSocket);
        WSACleanup();
        return -1;
    }

    // 连接到FTP服务器
    if (connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed.\n");
        closesocket(controlSocket);
        WSACleanup();
        return -1;
    }

    printf("Connected to FTP server.\n");
    return controlSocket;
}


int send_ftp_command(SOCKET controlSocket, const char *command) {
    // 发送FTP命令（带有CRLF结束符）
    char command_with_crlf[256];
    snprintf(command_with_crlf, sizeof(command_with_crlf), "%s\r\n", command);

    int result = send(controlSocket, command_with_crlf, (int)strlen(command_with_crlf), 0);
    if (result == SOCKET_ERROR) {
        printf("Failed to send command: %s\n", command);
        return -1;
    }

    printf("Command sent: %s", command);
    return 0;
}

int receive_ftp_response(SOCKET controlSocket, char *response, int response_size) {
    int bytesReceived = recv(controlSocket, response, response_size, 0);
    if (bytesReceived > 0) {
        response[bytesReceived] = '\0';  // 确保字符串以空字符结尾
        printf("Response: %s", response);
    } else if (bytesReceived == 0) {
        printf("Connection closed by server.\n");
    } else {
        printf("Recv failed.\n");
    }
    return bytesReceived;
}


int ftp_login(SOCKET controlSocket, const char *username, const char *password) {
    char response[1024];

    // 发送用户名
    send_ftp_command(controlSocket, "USER anonymous");  // 使用匿名登录
    receive_ftp_response(controlSocket, response, sizeof(response));

    // 发送密码
    send_ftp_command(controlSocket, "PASS anonymous");
    receive_ftp_response(controlSocket, response, sizeof(response));

    if (strncmp(response, "230", 3) == 0) {
        printf("Login successful.\n");
        return 0;
    } else {
        printf("Login failed.\n");
        return -1;
    }
}


int establish_data_connection(SOCKET controlSocket) {
    char response[1024];
    send_ftp_command(controlSocket, "PASV");  // 请求服务器进入被动模式
    receive_ftp_response(controlSocket, response, sizeof(response));

    // 解析响应中的IP和端口
    int ip[4], port[2];
    sscanf(strchr(response, '(') + 1, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);
    char data_ip[16];
    snprintf(data_ip, sizeof(data_ip), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    int data_port = port[0] * 256 + port[1];

    // 建立到数据端口的连接
    SOCKET dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in dataAddr;
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_port = htons(data_port);
    inet_pton(AF_INET, data_ip, &dataAddr.sin_addr);

    if (connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) == SOCKET_ERROR) {
        printf("Data connection failed.\n");
        closesocket(dataSocket);
        return -1;
    }

    printf("Data connection established.\n");
    return dataSocket;
}


int ftp_download_file(SOCKET controlSocket, const char *filename) {
    char response[1024];

    // 建立数据连接
    SOCKET dataSocket = establish_data_connection(controlSocket);
    if (dataSocket == -1) return -1;

    // 发送RETR命令
    char command[256];
    snprintf(command, sizeof(command), "RETR %s", filename);
    send_ftp_command(controlSocket, command);
    receive_ftp_response(controlSocket, response, sizeof(response));

    // 接收文件内容
    char buffer[1024];
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to create file.\n");
        closesocket(dataSocket);
        return -1;
    }

    int bytesReceived;
    while ((bytesReceived = recv(dataSocket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, file);
    }

    printf("File download complete.\n");
    fclose(file);
    closesocket(dataSocket);
    return 0;
}


int main() {
    const char *server_ip = "192.168.1.10";
    SOCKET controlSocket = connect_to_ftp_server(server_ip);
    if (controlSocket == -1) return 1;

    if (ftp_login(controlSocket, "anonymous", "anonymous") == -1) return 1;
    ftp_download_file(controlSocket, "testfile.txt");

    send_ftp_command(controlSocket, "QUIT");
    closesocket(controlSocket);
    WSACleanup();
    return 0;
}
