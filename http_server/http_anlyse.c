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
#include "http_anlyse.h"

#define port 8080

request_content_A_t request_content_A;  		//存储A类算法post数据

response_content_A_t response_content_A; 	//存储A类算法结果回调数据

result_json_t	result_json;

/* check_if_post    检查是否为post请求
 *
 * data   	    输入      报文数据
 * file_name    输出 	    需要操作的文件名
 * content	    输出	    报文包含的内容
 * return       返回    	不是post 返回-1  	是A类算法 返回 0  		B类算法返回	1      callback 返回    2
 */
int check_if_post(char * data, char * file_name, char * content)
{
	printf("enter check_if_post \r\n");
	int type = -1;
	char * head = NULL;
	char * tail = NULL;
	
	if(strncmp(data, "POST ", 5) == 0)
	{
//		head = data + 5;
		tail = strstr(data, " HTTP");
		if(tail == NULL){
			printf("### tail ERROR\r\n");
			return -1;
		}
		printf("tail %p\r\n", tail);
		strncpy(file_name, data + 5, tail - data - 5);
		
		head = strchr(data, '{');
		tail = strrchr(data, '}');
		if(head == NULL){
			printf("### head ERROR\r\n");
			return -1;
		}
		printf("head %p\r\n", tail);
		if(tail == NULL){
			printf("### ERROR tail\r\n");
			return -1;
		}
		printf("tail %p\r\n", tail);
		strncpy(content, head, tail - head + 1);
		type = 0;
	}
	else if(strncmp(data, "HTTP/", 5) == 0)
	{
		type = 2;
	}
	printf("type = %d\r\n", type);
	return type;
}

/* parse_post_A_content  解析A类算法post报文的content
 * data  输入  	报文的content数据
 */
int parse_post_A_content(char * data)
{
	printf("enter parse_post_A_content \r\n");
	char * requestHostIp_Start = NULL;
	char * requestHostIp_ValueStart = NULL;
	char * requestHostIp_EndStart = NULL;
	
	char * requestHostPort_Start = NULL;
	char * requestHostPort_ValueStart = NULL;

	char * requestId_Start = NULL;
	char * requestId_ValueStart = NULL;
	char * requestId_EndStart = NULL;

	char * objectId_Start = NULL;
	char * objectId_ValueStart = NULL;

	char * typeList_Start = NULL;
	char * typeList_ValueStart = NULL;
	char * typeList_EndStart = NULL;

	char * imageNormalPath_Start = NULL;
	char * imageNormalPath_ValueStart = NULL;
	char * imageNormalPath_EndStart = NULL;

	char * imageList_Start = NULL;
	char * imageList_ValueStart = NULL;
	char * imageList_EndStart = NULL;

	char * temp = NULL;
	int i = 0;

	requestHostIp_Start = strstr(data, "\"requestHostIp\":");
	if(requestHostIp_Start != NULL){
		requestHostIp_ValueStart = strchr(requestHostIp_Start + strlen("\"requestHostIp\":"), '\"');
		requestHostIp_EndStart = strchr(requestHostIp_ValueStart + 1, '\"');
		strncpy(request_content_A.request_host_ip, requestHostIp_ValueStart + 1, requestHostIp_EndStart - requestHostIp_ValueStart - 1);
	}else{
		printf("@11qqqq  ERROR\r\n ");
		return -1;
	}
	
	requestHostPort_Start = strstr(data, "\"requestHostPort\":");
	requestHostPort_ValueStart = strchr(requestHostPort_Start + strlen("\"requestHostPort\":"), '\"');
	sscanf(requestHostPort_ValueStart + 1, "%d", &request_content_A.request_host_port);

	requestId_Start = strstr(data, "\"requestId\":");
	requestId_ValueStart = strchr(requestId_Start + strlen("\"requestId\":"), '\"');
	requestId_EndStart = strchr(requestId_ValueStart + 1, '\"');
	strncpy(request_content_A.request_id, requestId_ValueStart + 1, requestId_EndStart - requestId_ValueStart - 1);

	objectId_Start = strstr(data, "\"objectId\":");
	objectId_ValueStart = strchr(objectId_Start + strlen("\"objectId\":"), '\"');
	sscanf(objectId_ValueStart + 1, "%d", &request_content_A.object_id);

	typeList_Start = strstr(data, "\"typeList\":");
	typeList_ValueStart = strchr(typeList_Start + strlen("\"typeList\":"), '\"');
	typeList_EndStart = strstr(typeList_ValueStart + 1, "\"");
	strncpy(request_content_A.type_list, typeList_ValueStart + 1, typeList_EndStart - typeList_ValueStart - 1);

	imageNormalPath_Start = strstr(data, "\"imageNormalPath\":");
	if(imageNormalPath_Start != NULL){
		imageNormalPath_ValueStart = strchr(imageNormalPath_Start + strlen("\"imageNormalPath\":"), '\"');
		imageNormalPath_EndStart = strstr(imageNormalPath_ValueStart + 1, "\"");
		strncpy(request_content_A.image_normal_path, imageNormalPath_ValueStart + 1, imageNormalPath_EndStart - imageNormalPath_ValueStart - 1);
	}
	
	imageList_Start = strstr(data, "\"imageList\":");
	imageList_ValueStart = strchr(imageList_Start + strlen("\"imageList\":"), '\"');
	imageList_EndStart = strstr(imageList_ValueStart + 1, "\"");
	strncpy(request_content_A.image_path_list, imageList_ValueStart + 1, imageList_EndStart - imageList_ValueStart - 1);

#if 1
	printf("========== parse_post_A_content ==========\r\n");
	printf("host_ip %s\t host_port %d\r\n", request_content_A.request_host_ip, request_content_A.request_host_port);
	printf("request_id %s\t \r\n", request_content_A.request_id);
	printf("object_id %d\t type_list %s\r\n", request_content_A.object_id, request_content_A.type_list);
	printf("image_normal_path %s\t image_path_list %s\r\n", request_content_A.image_normal_path, request_content_A.image_path_list);
	printf("==========================================\r\n");
#endif

	temp = strtok(request_content_A.type_list, "\",\"");
	i = 0;
	while(temp){
		sprintf(request_content_A.type_data.data[i], "%s", temp);
		i ++;
		temp = strtok(NULL, "\",\"");
	}
	request_content_A.type_data.count = i;

	temp = strtok(request_content_A.image_path_list, "\",\"");
	i = 0;
	while(temp){
		sprintf(request_content_A.image_path_list_data.data[i], "%s", temp);
		i ++;
		temp = strtok(NULL, "\",\"");
	}
	request_content_A.image_path_list_data.count = i;

	if(imageNormalPath_Start != NULL){
		temp = strtok(request_content_A.image_normal_path, "\",\"");
		i = 0;
		while(temp){
			sprintf(request_content_A.image_normal_path_data.data[i], "%s", temp);
			i ++;
			temp = strtok(NULL, "\",\"");
		}
		request_content_A.image_normal_path_data.count = i;
	}

	for(i = 0; i < request_content_A.type_data.count; i ++){
		printf("%s\t", request_content_A.type_data.data[i]);
	}
	for(i = 0; i < request_content_A.image_path_list_data.count; i ++){
		printf("%s\t", request_content_A.image_path_list_data.data[i]);
	}
	for(i = 0; i < request_content_A.image_normal_path_data.count; i ++){
		printf("%s\t", request_content_A.image_normal_path_data.data[i]);
	}
	printf("\r\n");
	return 0;
}

/* build_A_callback_content  构建A类算法结果回调post报文
 * buf 		输出		将报文保存在buf里
 * return	返回      返回报文的长度
 */
int build_A_callback_content(char * buf)
{
	int pos = 0;
	pos += sprintf(&buf[pos], "{");
	pos += sprintf(&buf[pos], "\"requestId\":\"%s\",", request_content_A.request_id);
	pos += sprintf(&buf[pos], "\"resultsList\":[{");
	pos += sprintf(&buf[pos], "\"objectId\":\"%d\",", request_content_A.object_id);
	pos += sprintf(&buf[pos], "\"results\":[{");
	pos += sprintf(&buf[pos], "\"type\":\"%s\",", result_json.type);
	pos += sprintf(&buf[pos], "\"value\":\"%d\",", result_json.value);
	pos += sprintf(&buf[pos], "\"code\":\"%d\",", result_json.code);
	pos += sprintf(&buf[pos], "\"resImagePath\":\"%s\",", result_json.res_image_path);
	
	pos += sprintf(&buf[pos], "\"pos\":[");
	for(int i = 0; i < result_json.count; i ++)
	{
		pos += sprintf(&buf[pos], "{\"areas\":[");
		for(int j = 0; j < result_json.area[i].count; j ++)
		{
			pos += sprintf(&buf[pos], "{\"x\":\"%lf\",\"y\":\"%lf\"}", result_json.area[i].x[j], result_json.area[i].y[j]);
			if(j + 1 < result_json.area[i].count){
				pos += sprintf(&buf[pos], ",");
			}
		}
		pos += sprintf(&buf[pos], "]}"); //areas
		if(i + 1 < result_json.count){
			pos += sprintf(&buf[pos], ",");
		}
	}
	pos += sprintf(&buf[pos], "]"); //pos
	pos += sprintf(&buf[pos], "}],"); //results
	pos += sprintf(&buf[pos], "\"conf\":\"%lf\",", result_json.conf);
	pos += sprintf(&buf[pos], "\"desc\":\"%s\"", result_json.desc);
	pos += sprintf(&buf[pos], "}]");   //resultList
	pos += sprintf(&buf[pos], "}");
	return pos;
}

/* build_callback_packet 构建返回包
 */
int build_callback_packet(char * buf, char * content, int content_len, char * file_name)
{
	int pos = 0;
		
	pos += sprintf(&buf[pos], "POST %s HTTP/1.1\r\n", file_name);
	pos += sprintf(&buf[pos], "HOST: 10.106.249.210:8080\r\n");
	pos += sprintf(&buf[pos], "Content-Type: %s\r\n", "application/json");
	pos += sprintf(&buf[pos], "Content-Length: %d\r\n", content_len);
	pos += sprintf(&buf[pos], "\r\n");
	pos += sprintf(&buf[pos], "%s", content);
//	pos += sprintf(&buf[pos], "\r\n");

//	printf("build_callback_packet : \r\n%s\r\n", buf);
	return pos;
}

/* build_error_callback  构建响应包
*/
int build_error_callback(char * buf, int error_type)
{
	int pos = 0;

	pos += sprintf(&buf[pos], "HTTP/1.1 %d ", error_type);
		
	switch(error_type)
	{
		case 200 : 
			pos += sprintf(&buf[pos], "%s", "OK\r\n");
			break;
			
		case 400 : 
			pos += sprintf(&buf[pos], "%s", "The client request has a syntax error, cannot be understood by the server.\r\n");
			break;

		case 500 : 
			pos += sprintf(&buf[pos], "%s", "The service side is abnormal.\r\n");
			break;

		default :
			goto EXIT;
	}

	pos += sprintf(&buf[pos], "%s", "Content-type: application/json\r\n");
	pos += sprintf(&buf[pos], "%s", "Content-length: 14\r\n\n");
	
	pos += sprintf(&buf[pos], "{\"code\":\"%d\"}", error_type);

	printf("build_error_callback send %s\r\n", buf);
	
	return pos;
	
EXIT:
	memset(buf, 0, pos);
	return -1;
}

/* parse_callback_packet  解析响应包
*/
int parse_callback_packet(char * data)
{
	int errcode = -1;
	char * errstr = NULL;
	sscanf(data, "HTTP/1.1 %d %s", &errcode, errstr);

	printf("callback return %d  %s \r\n", errcode, errstr);
	
	return 0;
}

int read_result_json(char * buf)
{
	printf("read_result_json start\r\n");

    // Open for reading result.json
    FILE* f = fopen("/root/result.json", "r");
    if (f == NULL) {
        printf("Failed to open result.json");
        return -1;
    }

    memset(buf, 0, 1024);
    int ret = fread(buf, 1, 1024, f);
    fclose(f);

    // Display the read contents from the file
    printf("Read from result.json: %s\r\n", buf);

	return ret;
}

int parse_result_json(char * data)
{
	printf("enter parse_result_json @@@@@\r\n");
	char * element_Start = NULL;
	char * element_ValueStart = NULL;
	char * element_EndStart = NULL;
	int i = 0, j = 0; 
	char * pos = NULL;
	char * area_Start = NULL;
	char * area_ValueStart = NULL;
	char * area_End = NULL;
	char * temp_xy_Start = NULL;
	char * temp_xy_ValueStart = NULL;
	char temp[512] = {0};
	
	element_Start = strstr(data, "\"type\":");
	if(element_Start != NULL){
		element_ValueStart = strchr(element_Start + strlen("\"type\":"), '\"');
		element_EndStart = strchr(element_ValueStart + 1, '\"');
		strncpy(result_json.type, element_ValueStart + 1, element_EndStart - element_ValueStart - 1);
	}

	element_Start = strstr(data, "\"resImagePath\":");
	if(element_Start != NULL){
		element_ValueStart = strchr(element_Start + strlen("\"resImagePath\":"), '\"');
		element_EndStart = strchr(element_ValueStart + 1, '\"');
		strncpy(result_json.res_image_path, element_ValueStart + 1, element_EndStart - element_ValueStart - 1);
	}

	element_Start = strstr(data, "\"desc\":");
	if(element_Start != NULL){
		element_ValueStart = strchr(element_Start + strlen("\"desc\":"), '\"');
		element_EndStart = strchr(element_ValueStart + 1, '\"');
		strncpy(result_json.desc, element_ValueStart + 1, element_EndStart - element_ValueStart - 1);
	}
	
	element_Start = strstr(data, "\"value\"");
	if(element_Start != NULL){
		element_ValueStart = strstr(element_Start + strlen("\"value\""), ":	");
		sscanf(element_ValueStart + strlen(":	"), "%d", &result_json.value);
	}
	
	element_Start = strstr(data, "\"code\"");
	if(element_Start != NULL){
		element_ValueStart = strchr(element_Start + strlen("\"code\""), ':');
		sscanf(element_ValueStart + strlen(": "), "%d", &result_json.code);
	}

	element_Start = strstr(data, "\"conf\"");
	if(element_Start != NULL){
		element_ValueStart = strchr(element_Start + strlen("\"conf\""), ':');
		sscanf(element_ValueStart + strlen(": "), "%lf", &result_json.conf);
	}

	pos = strstr(data, "\"pos\":");
	if(pos != NULL){
		area_Start = strstr(pos, "\"areas\":");
		i = 0;
		while(area_Start != NULL)
		{
			area_ValueStart = strchr(area_Start + strlen("\"areas\":"), '[');
			area_End = strchr(area_ValueStart, ']');
			strncpy(temp, area_ValueStart + 1, area_End - area_ValueStart - 1);

			temp_xy_Start = strstr(temp, "\"x\"");
			j = 0;
			while(temp_xy_Start != NULL)
			{
				temp_xy_ValueStart = strstr(temp_xy_Start + strlen("\"x\""), ":	");
				sscanf(temp_xy_ValueStart + strlen(":	"), "%lf", &result_json.area[i].x[j]);
				temp_xy_Start = strstr(temp_xy_ValueStart, "\"x\"");
				j ++;
			}

			temp_xy_Start = strstr(temp, "\"y\"");
			j = 0;
			while(temp_xy_Start != NULL)
			{
				temp_xy_ValueStart = strstr(temp_xy_Start + strlen("\"y\""), ":	");
				sscanf(temp_xy_ValueStart + strlen(":	"), "%lf", &result_json.area[i].y[j]);
				temp_xy_Start = strstr(temp_xy_ValueStart, "\"y\"");
				j ++;
			}
			result_json.area[i].count = j;
			
			area_Start = strstr(area_End, "\"areas\":");
			i ++;
		}
		result_json.count = i;
	}
#if 1
	printf("=============================================================\r\n");
	printf("type %s  value %d code %d conf %f\r\n", result_json.type,result_json.value, result_json.code, result_json.conf);
	printf("resImagePath %s  desc %s \r\n", result_json.res_image_path,result_json.desc);
	for(int i = 0; i < result_json.count; i ++){
		printf("area : \r\n");
		for(int j = 0; j < result_json.area[i].count; j++){
			printf("x , y : %lf , %lf \t", result_json.area[i].x[j], result_json.area[i].y[j]);
		}
	}
	printf("\r\n=============================================================\r\n");
#endif
	return 0;
}

/* socket_bind_listen	创建socket,绑定,监听
 * return 	返回 	sockfd 
 */
int socket_bind_listen()
{
	int opts = -1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		printf("socket error\r\n");
		return -1;
	}

	opts = fcntl(sockfd, F_GETFL, 0);
	if(opts < 0){
		printf(">>>>>> F_GETFL error 1\r\n");
		return -1;
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sockfd, F_SETFL, opts) < 0){
		printf(">>>>>> F_SETFL error 2 \r\n");
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(port);  
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	printf("port %d\r\n", port);
	int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)//防止地址占用
    {
        printf("Server setsockopt failed\r\n");
        return -4;
    }
	
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

int tcp_connect(char * request_ip, int request_post)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("socket error\r\n");
		return -1;
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(request_ip);
	server_addr.sin_port = htons(request_post);
	printf("IP  port  %s : %d\r\n", request_ip, request_post);
	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("connect error\r\n");
		return -2;
	}
	return sockfd;
}

int main()
{
	char buf[1024] = {0};
	int content_len = 0, packet_len = 0;
	
//	fd_set rset, allset;
	char file_name[128] = {0};
	char content[512] = {0};
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
					printf("ret = %d\r\n", ret);
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
						char * is_one_packet = strchr(buf, '{');
						if(is_one_packet == NULL){
							ret = recv(tmpfd, buf + strlen(buf), 1024 - strlen(buf), 0);
							if(ret < 0){
								printf("receive error\r\n");
								return -1;
							}
						}
						printf("ip:port %s:%d client[%d] send :\r\n%s\r\n", client_ip[i], client_port[i], tmpfd, buf);
						//解析
						ret = check_if_post(buf, file_name, content);
						if(ret == -1)
						{
							printf("receive data not post\r\n");
							memset(buf, 0, 1024);
							len = build_error_callback(buf, 400);
							ret = send(tmpfd, buf, len, 0);
							if(ret < 0){
								printf("check_if_post error 400 send error\r\n");
							}
							continue;
						}
						else if(ret == 0){
							parse_post_A_content(content);
						}
						else if(ret == 2){
							parse_callback_packet(buf);
						}
						memset(content, 0, 512);
						memset(buf, 0, 1024);
						
						// 响应
						len = build_error_callback(buf, 200);
						ret = send(tmpfd, buf, len, 0);
						if(ret < 0){
							printf("build_error_callback  send error\r\n");
							break;
						}
//						memset(&request_content_A, 0, sizeof(struct request_content_A_t));
						
/*===========================================================================================*/
						// 获取图片
//						int sendfd = tcp_connect(request_content_A.request_host_ip, request_content_A.request_host_port);
						char * image_path = "/root/demo/data/npu_image/rgb/test.jpg.rgb";
						char * model_path = "/root/demo/data/npu_model/detection/yolov8_onnx.npubin";
						memset(buf, 0, 1024);
//						sprintf(buf, "%s.rgb", request_content_A.image_path_list);
//						ret = rename(request_content_A.image_path_list, buf);
//						if(ret < 0){
//							printf("rename error\r\n");
//						}
//						memset(buf, 0, 1024);
						
						//todo 调用 算法demo
						sprintf(buf, "/root/demo/lenovo_alg ./image/%s.rgb %s %s", request_content_A.image_path_list, model_path, request_content_A.type_list);
//						sprintf(buf, "/root/demo/lenovo_alg %s %s %s", image_path, model_path, "aqmzc");
						system(buf);
						
						//todo 读取result.json 文件 解析返回数据
						memset(buf, 0, 1024);
						read_result_json(buf);
						parse_result_json(buf);
						printf("request_id %s object_id %d\r\n", request_content_A.request_id, request_content_A.object_id);
						response_content_A.object_id = request_content_A.object_id;
						memcpy(response_content_A.request_id, request_content_A.request_id, strlen(request_content_A.request_id));
						memcpy(result_json.res_image_path, image_path, strlen(image_path));

						//回复
						content_len = build_A_callback_content(content);
						
						memset(buf, 0, 1024);
						packet_len = build_callback_packet(buf, content, content_len, "/platform/api/v1/task/results/pic-result-notify");
						int sendfd = tcp_connect(request_content_A.request_host_ip, request_content_A.request_host_port);
						ret = send(sendfd, buf, packet_len, 0);
						//ret = send(sendfd, content, content_len, 0);
						if(ret < 0){
							printf("build_callback_packet  send error\r\n");
							break;
						}
						printf("send %s\r\n", buf);
						memset(buf, 0, 1024);
						ret = recv(sendfd, buf, 1024, 0);
						printf("recv %s\r\n", buf);
						close(sendfd);
						
						memset(content, 0, 512);
						memset(buf, 0, 1024);
						memset(&result_json, 0, sizeof(struct result_json_t));
						memset(&request_content_A, 0, sizeof(struct request_content_A_t));
						memset(&response_content_A, 0, sizeof(struct response_content_A_t));
					}
					
					if (--nready == 0) break; 
				}
			}
		}
		close(sockfd);
	}
	return 0;
}


