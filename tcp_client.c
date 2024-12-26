/*!
    \file  tcp_client.c
    \brief TCP client demo program

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "tcp_client.h"
#include <string.h>
#include <stdio.h>
#include "gd32f30x.h"
#include "main.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/api.h"
#include "app_ota.h"
#include "app_configure.h"
#include "app_rs485net.h"
#include "app_service_console.h"
#include "netdb.h"

#ifdef CLIENT_DEBUG
#define clientlog(format, ...) \
  			printf("[%s:%d] "format, __func__, __LINE__,  ##__VA_ARGS__)
#else
#define	clientlog(format, ...)
#endif

#define TCP_CLIENT_TASK_PRIO            ( tskIDLE_PRIORITY + 7)
#define MAX_BUF_SIZE                    800
#define TIME_WAITING_FOR_CONNECT        ( ( portTickType ) 500 )

#define NET_IP "www.baidu.com"//"153.3.238.102"  
#define NET_PORT 80

#if ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1))

struct recev_packet
{
    int length;
    char bytes[MAX_BUF_SIZE];
};

static err_t tcp_client_recv(struct netconn *conn, struct netbuf *buf);

/*!
    \brief      called when a data is received
    \param[in]  conn: the TCP netconn over which to send data
    \param[in]  buf: the received data
    \param[out] none
    \retval     err_t: error value
*/
static err_t tcp_client_recv(struct netconn *conn, struct netbuf *buf)
{
    struct pbuf *q;
    struct recev_packet recev_packet;
    uint32_t data_len = 0;


    /* we perform here any necessary processing on the buf */
    if (NULL != buf) {

        for (q = buf->p; q != NULL; q = q->next) {
            /* if the received data size is larger than the size we want to get */
            if (q->len > (MAX_BUF_SIZE - data_len)) {
                /* only copy MAX_BUF_SIZE bytes data */
                memcpy((recev_packet.bytes + data_len), q->payload, (MAX_BUF_SIZE - data_len));
                data_len = MAX_BUF_SIZE;
            } else {
                /* copy q->len bytes data */
                memcpy((recev_packet.bytes + data_len), q->payload, q->len);
                data_len += q->len;
            }

            if (data_len >= MAX_BUF_SIZE) {
                break;
            }
        }

        recev_packet.length = data_len;
        netconn_write(conn, (void *) & (recev_packet.bytes), recev_packet.length, NETCONN_COPY);
    }

    return ERR_OK;
}

/*!
    \brief      tcp_client task
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/
static void tcp_client_task(void *arg)
{
    struct netconn *conn;
    struct ip_addr ipaddr;
    struct netbuf *buf;
    err_t ret, recv_err;

    IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);

    while (1) {
        /* creat TCP connection */
        conn = netconn_new(NETCONN_TCP);
        /* bind the new netconn to any IP address and port 1026 */
        recv_err = netconn_bind(conn, IP_ADDR_ANY, 1026);

        if ((ERR_USE != recv_err) && (ERR_ISCONN != recv_err)) {

            /* connect the new netconn to remote server and port 1026 */
            ret = netconn_connect(conn, &ipaddr, 1026);
            if (ERR_OK == ret) {

                recv_err = netconn_recv(conn, &buf);
                while (ERR_OK == recv_err) {

                    /* handle the received data */
                    tcp_client_recv(conn, buf);
                    netbuf_delete(buf);

                    recv_err = netconn_recv(conn, &buf);
                }
            }
        }

        /* close connection and discard connection identifier */
        netconn_close(conn);
        netconn_delete(conn);
    }
}

#endif /* ((LWIP_SOCKET == 0) && (LWIP_NETCONN == 1)) */



#if LWIP_SOCKET
#include "lwip/sockets.h"

/*!
    \brief      tcp_client task
    \param[in]  arg: user supplied argument
    \param[out] none
    \retval     none
*/



int i = 0;
int  ret, recvnum, sockfd = -1;
int tcp_port = 8080;
struct sockaddr_in svr_addr, clt_addr;
unsigned char buf[MAX_BUF_SIZE];
//    uint16_t i = 0;
struct ip_addr ipaddr;

int received = 0;
int remain = MAX_BUF_SIZE;

uint8_t tcp_client_flag = 0;
uint8_t net_connect_flag = 0;

//int read_data(int sockfd, char* buffer, size_t size) {
//    int len = recv(sockfd, buffer, size, MSG_WAITALL);
//    return len;
//}


//void receive_data(int sockfd) {
//	int fd = 0;
//    unsigned char buffer[BUFFER_SIZE];
//    char fileName[256] = {0};
//    int bytesRead = 0;
//    long long fileSize = 0;
//    int totalBytesRead = 0;
//    int packetBytesRead = 0;
//	
//	while (1) {
//        memset(buffer, 0, sizeof(buffer));
//        bytesRead = read_data(sockfd, buffer, BUFFER_SIZE);
//        if (bytesRead <= 0) {
//            break;
//        }
//        
//        if (buffer[0] == 0xff && buffer[1] == 0xdd) {  // WAV ?????
//            memset(buffer, 0, sizeof(buffer));
//            bytesRead = read_data(sockfd, buffer, PACKET_SIZE);
//			clientlog("bytesRead=%d\n", bytesRead);
//			break;
//        } else {
//            clientlog("Invalid header\n");
//            break;
//        }
//    }
//    
//    //close(sockfd);

//}

extern uint8_t SN_FLAGE;
static void tcp_client_task(void *arg)
{
	if(!SN_FLAGE){
		IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);
	}else{
#if 0
		IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);
#else
		if(p_configuration_data->server_ip.ipaddr0 != 0 && p_configuration_data->server_ip.ipaddr1 != 0){
			IP4_ADDR(&ipaddr, p_configuration_data->server_ip.ipaddr0, p_configuration_data->server_ip.ipaddr1, 
							  p_configuration_data->server_ip.ipaddr2, p_configuration_data->server_ip.ipaddr3);
		}else{
			IP4_ADDR(&ipaddr, IP_S_ADDR0, IP_S_ADDR1, IP_S_ADDR2, IP_S_ADDR3);	
		}
#endif
	}
	
    /* set up address to connect to */
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(tcp_port);
    svr_addr.sin_addr.s_addr = ipaddr.addr;

    clt_addr.sin_family = AF_INET;
    clt_addr.sin_port = htons(tcp_port);
    clt_addr.sin_addr.s_addr = htons(INADDR_ANY);

    while (1) 
	{
        /* create the socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            continue;
        }

        ret = bind(sockfd, (struct sockaddr *)&clt_addr, sizeof(clt_addr));
        if (ret < 0) {
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }

        /* connect */
        ret = connect(sockfd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
        if (ret < 0) {
            lwip_close(sockfd);
            sockfd = -1;
            continue;
        }
		//receive_data(sockfd);
		
        while (-1 != sockfd) 
		{
            /* reveive packets, and limit a reception to MAX_BUF_SIZE bytes */
            memset(buf, 0xff, sizeof(buf));
            recvnum = recv(sockfd, buf, MAX_BUF_SIZE, 0);
            if (recvnum <= 0) {
                lwip_close(sockfd);
                sockfd = -1;
                break;
            }
			
			
//            clientlog("receive data num :%d\r\n", recvnum);
//            for (i = 0; i < recvnum; i++) {
//                clientlog("0x%02X ", buf[i]);
//                if (15 == i % 16) {
//                    clientlog("\r\n");
//                }
//            }
//            clientlog("\r\n");    

		/*check if data is ota*/
            if (handle_otadata(buf) != 2)
            {
                send(sockfd, buf, 3, 0);
                continue;
            }
		
			if (handle_configuration_read(buf) != 2)
			{
				send(sockfd, buf, RECV_DATA_NUM, 0);
				
				buf[0] = 0xBB;
				buf[1] = 0xFF;
				buf[2] = 0x00;
				send(sockfd, buf, 3, 0);
				continue;
			}
		
			if (handle_configuration_write(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
		
			if(handle_configuration_downloaded_wav(buf) != 7)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}	
			
			if(handle_mac_addr_write(buf) != -1)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(handle_mac_addr_read(buf) != 0)
			{
				send(sockfd, buf, 8, 0);
				continue;
			}
			//test part

			if (skip_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (uart0_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (uart1_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (uart2_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (uart3_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (liren_test(buf) != 2)
			{
				send(sockfd, buf, 6, 0);
				continue;
			}
			
			if (hugan_test(buf) != 2)
			{
				send(sockfd, buf, 6, 0);
				continue;
			}
			
			if (dddq_key_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (display_test(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if (led_test(buf) != 2)
			{
				continue;
			}
			
			if (audio_test(buf) != 2)
			{
				continue;
			}
			
			if (set_pcba_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(read_pcba_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 4, 0);
				continue;
			}

			if (set_control_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(read_control_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 4, 0);
				continue;
			}
			
			if (set_pre_burn_in_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(read_pre_burn_in_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 4, 0);
				continue;
			}
			
			if (set_complete_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(read_complete_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 4, 0);
				continue;
			}
			
			if(read_aging_pass_flag(buf) != 2)
			{
				send(sockfd, buf, 4, 0);
				continue;
			}
			
			if(set_device_sn(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(read_device_sn(buf) != 2)
			{
				send(sockfd, buf, 34, 0);
				continue;
			}
			
			
			if(enter_factory_mode(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			
			if(clear_aging_flag(buf) != 2)
			{
				send(sockfd, buf, 3, 0);
				continue;
			}
			
			if(set_device_rtc(buf) != 2)
			{
				send(sockfd, buf, 2, 0);
				continue;
			}
			
			if(read_device_rtc(buf) != 2)
			{
				send(sockfd, buf, 6, 0);
				continue;
			}
			
			if(rotary_left_test(buf) != 2)
			{
				send(sockfd, buf, 10, 0);
				continue;
			}
			
			
			if(rotary_right_test(buf) != 2)
			{
				send(sockfd, buf, 10, 0);
				continue;
			}
			
		}
		lwip_close(sockfd);
		sockfd = -1;
    }
}
	

#endif /* LWIP_SOCKET */


/*!
    \brief      initialize the tcp_client application
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tcp_client_init(void)
{
    xTaskCreate(tcp_client_task, "TCP_CLIENT", DEFAULT_THREAD_STACKSIZE, NULL, TCP_CLIENT_TASK_PRIO, NULL);
}

int check_net(void)
{
	int fd; 
	int in_len=0;
	struct sockaddr_in servaddr;
	struct hostent *l_hostent;
	
	l_hostent = lwip_gethostbyname(NET_IP);
	
	in_len = sizeof(struct sockaddr_in);
	clientlog("socket*****\r\n");
	fd = socket(AF_INET,SOCK_STREAM,0);
	clientlog("socket done*****\r\n");
	if(fd < 0){   
		net_connect_flag = 0;
		clientlog("********** socket error *************\r\n");
		return -1; 
	}   

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(NET_PORT);
	//memcpy(&servaddr.sin_addr.s_addr, *l_hostent->h_addr_list, l_hostent->h_length);
	memcpy(&servaddr.sin_addr.s_addr, *l_hostent->h_addr_list, strlen(*l_hostent->h_addr_list));
	
//	servaddr.sin_addr.s_addr = inet_addr(NET_IP);
	clientlog("baidu ip %d.%d.%d.%d \r\n", (int)(servaddr.sin_addr.s_addr)&0xff,(int)(servaddr.sin_addr.s_addr>>8)&0xff,(int)(servaddr.sin_addr.s_addr>>16)&0xff,(int)(servaddr.sin_addr.s_addr>>24)&0xff);
	memset(servaddr.sin_zero,0,sizeof(servaddr.sin_zero));

	if(connect(fd,(struct sockaddr* )&servaddr,in_len) < 0 ) 
	{   
		close(fd);
		net_connect_flag = 0;
		clientlog("********** connect error *************\r\n");
		return -2; //没有联网成功
	}   
	else
	{   
		close(fd);
		net_connect_flag = 1;
		clientlog("********** connect success *************\r\n");
		return 1;
	}
}


