#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <string.h>
#include <stdio.h>
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/api.h"

#ifdef SERVER_DEBUG
#define serverlog(format, ...) \
  			printf("[%s:%d] "format, __func__, __LINE__,  ##__VA_ARGS__)
#else
#define	serverlog(format, ...)
#endif


int client[10] = {0};

static void tcp_server_task(void *arg)
{
	int i, n, maxi;
	int port = 24416;
    int nready;                 /* �Զ�������client, ��ֹ����1024���ļ�������  FD_SETSIZEĬ��Ϊ1024 */
    int maxfd, listenfd, connfd, sockfd;
    uint8_t buf[512];         /* #define INET_ADDRSTRLEN 16 */

    struct sockaddr_in clie_addr, serv_addr;
    socklen_t clie_addr_len;
    fd_set rset, allset;                            /* rset ���¼��ļ����������� allset�����ݴ� */
	while(1)
	{
		int opts, index;
		listenfd = socket(AF_INET, SOCK_STREAM, 0);

		opts = fcntl(listenfd, F_GETFL, 0);
		if(opts < 0){
			serverlog(">>>>>> F_GETFL error 1\r\n");
			continue;
		}
		opts = opts | O_NONBLOCK;
		if(fcntl(listenfd, F_SETFL, opts) < 0){
			serverlog(">>>>>> F_SETFL error 2 \r\n");
			continue;
		}
		
		serv_addr.sin_family= AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port= htons(port);
		
		bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		listen(listenfd, 10);

		maxfd = listenfd;                                           /* ��� listenfd ��Ϊ����ļ������� */

		maxi = -1;                                                  /* ��������client[]���±�, ��ʼֵָ��0��Ԫ��֮ǰ�±�λ�� */
		memset(client, -1, 10);                                        /* ��-1��ʼ��client[] */

		FD_ZERO(&allset);
		FD_SET(listenfd, &allset);                                  /* ����select����ļ��������� */

		while (1) {   
			rset = allset;                                          /* ÿ��ѭ��ʱ����������select����źż� */
			nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
			if (nready < 0){
				serverlog("select error\r\n");
				break;
			}
			if (FD_ISSET(listenfd, &rset)) 
			{                        /* ˵�����µĿͻ����������� */
				clie_addr_len = sizeof(clie_addr);
				connfd = accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);       /* Accept �������� */
				serverlog("accept ...\r\n");
				
				for (i = 0; i < 10; i++){
					if (client[i] < 0) {                            /* ��client[]��û��ʹ�õ�λ�� */
						client[i] = connfd;                         /* ����accept���ص��ļ���������client[]�� */
						break;
					}
				}
				if (i == 10) {                              /* �ﵽselect�ܼ�ص��ļ��������� 1024 */
					serverlog("too many clients\r\n");
					break;
				}

				FD_SET(connfd, &allset);                            /* �����ļ�����������allset�����µ��ļ�������connfd */
				if (connfd > maxfd)
					maxfd = connfd;                                 /* select��һ��������Ҫ */

				if (i > maxi)
					maxi = i;                                       /* ��֤maxi�������client[]���һ��Ԫ���±� */

				if (--nready == 0)
					continue;
			} 
			
			for (i = 0; i <= maxi; i++) 
			{                               /* ����ĸ�clients �����ݾ��� */
				if ((sockfd = client[i]) < 0)
					continue;
				if (FD_ISSET(sockfd, &rset)) 
				{
					n = recv(sockfd, buf, sizeof(buf), 0);
					if ( n== 0) {    /* ��client�ر�����ʱ,��������Ҳ�رն�Ӧ���� */
						serverlog("disconnected!!\r\n");
						
						lwip_close(sockfd);
						FD_CLR(sockfd, &allset);                        /* ���select�Դ��ļ��������ļ�� */
						client[i] = -1;
					} 
					else if (n > 0)
					{
						uint32_t timestamp = rtc_counter_get();
						uint8_t index = 0;
						handle_recv_data(sockfd, buf);
						serverlog("handle_recv_data-----\r\n");
			
						if(p_configuration_data->camera_type == 1){
							handle_record_status();
							handle_plug_flow();
							
							for(index = 0; index < 4; index ++){    //����ͷ��ʱ�䲻������  ��Ϊ����
								if((dhlr_group[index].timestramp != 0)
									&& (timestamp - dhlr_group[index].timestramp > 20)){
									dhlr_group[index].online = 0;
									dhlr_group[index].timestramp = 0;
								}
							}
						}
						
					}
					if (--nready == 0)
						break;                                          /* ����for, ������while�� */
				}
			}
			
		}
		lwip_close(listenfd);
	}
}	

