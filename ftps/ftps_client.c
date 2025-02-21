#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <iconv.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
 
//settings
#define IMPLICIT_CONN 0
#define EXPLICIT_CONN 1
 
//ftp
#define TYPE_A_CMD "TYPE A\r\n"
#define TYPE_I_CMD "TYPE I\r\n"
#define PASV_CMD "PASV\r\n"
#define PROT_P_CMD "PROT P\r\n"
#define PBSZ_0_CMD "PBSZ 0\r\n"
#define AUTH_SSL_CMD "AUTH SSL\r\n"
#define AUTH_TLS_CMD "AUTH_TLS\r\n"
#define QUIT_CMD "QUIT\r\n"

//socket
#define SETSOCKOPT_OPTVAL_TYPE (void *)
 
//ssl/tls
#define _FTPLIB_SSL_CLIENT_METHOD_ TLSv1_2_client_method
typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct bio_st BIO;
typedef struct x509_st X509;
//typedef bool (*FtpCallbackCert)(void *arg, X509 *cert);
 
//ftp cmd codemapping
#define DOWNLOAD 0
#define UPLOAD   1
#define LS_AL    2
 
typedef struct _ftps_client_{
	SSL * ssl;
	SSL_CTX* ctx;
	BIO* sbio;
	int connfd;
	int mode;/** pasv or port**/
	char ip[16];
	X509 *cert;
}FtpsClient;
 
 
/**============================static function declaration==============================================**/
static int SetConnType(FtpsClient *cli, const int type);
static int CloseFtpsDataConn(FtpsClient *cli,FtpsClient *datahandle);
 
/**==================================================public code segment start==================================================**/
 
/**
测试连接是否可用
**/
int TestConnection(int sock)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int32_t ret = getsockname(sock, (struct sockaddr *)&addr, &addr_len);
    if(ret == 0)
    {
        printf("getsockname succ:%s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    else
    {
        printf("getsockname failed,error=%d\n", errno);
        
    }
 
    memset(&addr, 0, sizeof(addr));
    ret = getpeername(sock, (struct sockaddr *)&addr, &addr_len);
    if(ret == 0)
    {
        printf("getpeername succ:%s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    else
    {
        printf("getpeername failed,error=%d\n", errno);
        return -1;
    }
    
    return 0;
}
 
/*
函数：打开socket 客户端连接,默认与服务端连接超时5秒
*/
int OpenConnectByIp(const char* ip, const char* port)
{
 
	struct sockaddr_in sa;
	//    fd_set             rSet;
	int                connfd = 0;
	//    int                arg = 0;
 
	memset(&sa, 0, sizeof(sa));
 
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ip);
	sa.sin_port = htons(atoi(port));
 
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connfd < 0) {
		return -1;
	}
	/*
		struct timeval timeo = { 5, 0 };
		arg = fcntl(connfd, F_GETFL, NULL);
		arg |= O_NONBLOCK;
		fcntl(connfd, F_SETFL, arg);
	*/
	int r = connect(connfd, (struct sockaddr*)&sa, sizeof(sa));
	if (r) {
		perror("sock connect:");
		fprintf(stderr, "connect err retcode:%d", r);
		/*
				FD_ZERO(&rSet);
				FD_SET(connfd, &rSet);
				if (select(connfd + 1, NULL, &rSet, NULL, &timeo) <= 0)
				{
					close(connfd);
				}
		*/
		close(connfd);
		return -1;
	}
 
	return connfd;
}
 
 
 
/**
ip:port或者server:port的方式进行连接
**/
int OpenConnectByHost(FtpsClient *cli, const char *host)
{
	struct sockaddr_in sin;
	struct linger lng = { 0, 0 };
	struct hostent *phe;
	struct servent *pse;
	int sControl = 0;
	int on=1;
	int ret;
	char lhost[24];
	char *pnum;
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	//lhost = strdup(host);
	memcpy(lhost, host, strlen(host));
	pnum = strchr(lhost, ':');
	if (pnum == NULL)
	{
		if ((pse = getservbyname("ftp","tcp")) == NULL)
		{
			perror("getservbyname");
			return 0;
		}
		sin.sin_port = pse->s_port;
	}
	else
	{
		*pnum++ = '\0';
		if (isdigit(*pnum)) {
			sin.sin_port = htons(atoi(pnum));
		}
		else
		{
			pse = getservbyname(pnum,"tcp");
			sin.sin_port = pse->s_port;
		}
	}
	printf("getservbyname success\n");	
		
	ret = inet_aton(lhost, &sin.sin_addr);
	if (ret == 0){
		if ((phe = gethostbyname(lhost)) == NULL)
		{
			perror("gethostbyname");
			return -1;
		}
		memcpy((char *)&sin.sin_addr, phe->h_addr, phe->h_length);
		snprintf(cli->ip,sizeof(cli->ip),"%s",inet_ntoa(*(struct in_addr*)phe->h_addr_list[0])	);	
	}else
		snprintf(cli->ip,sizeof(cli->ip),"%s",lhost);	
 
	sControl = socket(AF_INET, SOCK_STREAM, 0);
	if (sControl == -1)
	{
		perror("socket");
		return -1;
	}
	
	if (setsockopt(sControl,SOL_SOCKET,SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE &on, sizeof(on)) == -1)
	{
		perror("setsockopt");
		return -1;
	}
	
	if (setsockopt(sControl,SOL_SOCKET,SO_LINGER, SETSOCKOPT_OPTVAL_TYPE &lng,sizeof(lng)) == -1)
	{
		perror("setsockopt");
		close(sControl);
		return -1;
	}
	
 
	if (connect(sControl, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		perror("connect");
		return -1;
	}
	
	cli->connfd = sControl;
	
	printf("socket connect success,connect fd = %d\n", cli->connfd);	
	
	return 0;
	
}
 
/**==================================================public code segment end==================================================**/
 
 
 
/**==================================================FTP+SSL/TLS or FTPS code segment start==================================================**/
 
 
/**
初始化SSL
**/
int InitSSL(FtpsClient *cli)
{
	printf("enter initssl\r\n");
	SSL_load_error_strings();
	SSL_library_init();

	cli->ctx = SSL_CTX_new(TLSv1_2_client_method());
	if(cli->ctx == NULL){
		printf("SSL_CTX_new SSLv23_client_method = NULL\r\n");
		return -1;
	}
	SSL_CTX_set_verify(cli->ctx, SSL_VERIFY_NONE, NULL);

	cli->ssl = SSL_new(cli->ctx);
	if(cli->ctx == NULL){
		printf("SSL_CTX_new SSLv23_client_method = NULL\r\n");
		return -1;
	}

	return 0;	
}
 
/**
SSL握手
**/
int NegoSSL(FtpsClient *cli)
{
	printf("enter NegoSSL\r\n");

	int ret = 0;
	long iErr = 0;
	char *pErr = NULL;
	cli->sbio = BIO_new_socket(cli->connfd, BIO_NOCLOSE);
	SSL_set_bio(cli->ssl,cli->sbio,cli->sbio);

	SSL_set_fd(cli->ssl, cli->connfd);

#if 1	
	ret = SSL_connect(cli->ssl);    ///////////////////////////////////////////////

	printf("ssl_connect %s  ret = %d \n ", ret == 1 ? "success" : "failed", ret);
	if(ret <= 0){
		ret = SSL_get_error(cli->ssl, ret);
		printf("SSL_get_error return code[%d]\n",ret);
		ERR_print_errors_fp(stdout);
		iErr = ERR_get_error();
		pErr = ERR_error_string(ret, NULL);
		printf("pErr=%s\n",pErr);
		return -1;
	}
#else
	SSL_set_connect_state(cli->ssl);
	SSL_do_handshake(cli->ssl);
#endif
	cli->cert = SSL_get_peer_certificate(cli->ssl);
	return 0;
}
 
/**
连接成功后的回应,并打印
**/
int PrintResp(FtpsClient *cli)
{
	char recvbuf[1024];bzero(recvbuf,sizeof(recvbuf));
	int ret = SSL_read(cli->ssl, recvbuf, sizeof(recvbuf));
	printf("Resp:ret = %d:%s", ret, recvbuf);
	if(ret <= 0){
	ret = SSL_get_error(cli->ssl, ret);
		printf("SSL_get_error return code[%d]\n",ret);
		ERR_print_errors_fp(stdout);
		long iErr = ERR_get_error();
		char * pErr = ERR_error_string(ret, NULL);
		printf("pErr=%s\n",pErr);
	}
	return 0;	
}
 
/**
封装了ssl_read
**/
int EnRecv(FtpsClient *cli,char *recvbuf,int recvlen)
{
	return SSL_read(cli->ssl, recvbuf, recvlen);
}
 
/**
封装SSL_write
**/
int EnSend(FtpsClient *cli,const char *sendbuf,int sendlen)
{
	int ret = SSL_write(cli->ssl, sendbuf, sendlen);
	if(ret <= 0){
		printf("SSL_write error %d\r\n", ret);
		return -1;
	}	
	return 0;	
}
 
 
 
/** 
创建ftps的控制链路(不发送AUTH TLS/SSL的指令)
**/
int FtpsConn(FtpsClient *cli,const char *host)
{
	int ret = 0;
//	printf("start FtpsConn\r\n");
	ret = OpenConnectByHost(cli, host);
	if(ret)
		return -1;
	ret = InitSSL(cli);
	if(ret)
		return -1;	
	ret = NegoSSL(cli);	
	if(ret)
		return -1;	

	return 0;	
}
 
/**
登录,SSL加密登录
**/
int FtpsLogin(FtpsClient *cli,const char *user,const char *pass)
{
	printf("start FtpsLogin\r\n");
	char buff[1024];
	bzero(buff,sizeof(buff));
	SetConnType(cli,1);	
	snprintf(buff,sizeof(buff),"USER %s\r\n",user);
	EnSend(cli,buff,strlen(buff));
	PrintResp(cli);
	
	snprintf(buff,sizeof(buff),"PASS %s\r\n",pass);
	EnSend(cli,buff,strlen(buff));
	PrintResp(cli);

	return 0;
}
 
/**
设置隐式or显式连接
**/
int SetConnType(FtpsClient *cli, const int type)
{
	char rbuff[1024];
	bzero(rbuff,sizeof(rbuff));
	printf("enter SetConnType\r\n");
		
	EnSend(cli,PROT_P_CMD,strlen(PROT_P_CMD));
	PrintResp(cli);	
	
	EnSend(cli,PBSZ_0_CMD,strlen(PBSZ_0_CMD));
	PrintResp(cli);
	/** 设置与FTPS server的交互属性 END **/
	
	if(EXPLICIT_CONN == type){
		/** 显示连接 发送AUTH SSL/TLS **/
		EnSend(cli,AUTH_SSL_CMD,strlen(AUTH_SSL_CMD));
		PrintResp(cli);
		EnSend(cli,AUTH_TLS_CMD,strlen(AUTH_TLS_CMD));
		PrintResp(cli);
	}
	
	return 0;
	
}
 
/**
host format:"server:port"
**/
FtpsClient* OpenFtpsClient(const char *host)
{
	int failed = 0;
	
	FtpsClient * cli = (FtpsClient *)malloc(sizeof(FtpsClient) + 1);
	if(cli == NULL){
		perror("FtpsClient control handle malloc err\n");	
		return NULL;
	}
	printf("OpenFtpsClient cli size = %d addr %p\r\n", sizeof(FtpsClient), cli);
	
	failed = FtpsConn(cli, host);
	if(failed == -1){
		return NULL;
	}
	
	PrintResp(cli);
	return cli;
}
 
 
 
 
/**
开启被动连接时socket握手
**/
int FtpsOpenPasvSockConn(FtpsClient *cli,FtpsClient *datahandle)
{
	int ret = 0;
	char host[256];
	int port = 0;
	char *cp,buff[512],str1[32],str2[32],str3[32];
	int v[7];
 
	
	/** 被动模式需要另外打开一条数据链路 **/
	do{
 
		bzero(buff,sizeof(buff));
		bzero(v,sizeof(v));
		EnSend(cli,PASV_CMD,strlen(PASV_CMD));/** 查询被动端口 **/
//		PrintResp(cli);
		EnRecv(cli,buff,sizeof(buff));
		printf("PASV port :%s\r\n",buff);
		cp = strchr(buff,'(');
		if (cp == NULL) {
			printf("PASV return may some error\r\n");
			return -1;
		}	
		cp++;
	
		sscanf(buff,"%d %s %s %s (%d,%d,%d,%d,%d,%d).",&v[0],str1,str2,str3,&v[1],&v[2],&v[3],&v[4],&v[5],&v[6]);        
		
		port=v[5]*256+v[6];
		printf("server ip %s port=%d\n", cli->ip, port);
		
		snprintf(host,sizeof(host),"%s:%d",cli->ip,port);		
		
		ret = OpenConnectByHost(datahandle,host);
		if(ret < 0)
			printf("Host:%s data sock connect failed,To retry another port...\n",host);
		else
			break;	
		sleep(5);
	}while(ret < 0);
	
//	TestConnection(datahandle->connfd);
	
	return 0;
}
 
 
/**
开启时被动连接时的SSL握手
**/
int FtpsOpenPasvSslConn(FtpsClient *cli,FtpsClient *datahandle)
{
	int success = 0;
	int failed = 0;
	char *perr = NULL;
	datahandle->ssl = SSL_new(cli->ctx);
	if(datahandle->ssl == NULL){
		printf("pdatacli->ssl err\n");
		close(datahandle->connfd);
		return -1;	
	}
	SSL_set_fd(datahandle->ssl,datahandle->connfd);
	success = SSL_connect(datahandle->ssl);
	printf(" ssl_connect %s  success = %d \n ",success==1?"success":"failed",success);
	if (success != 1){
		failed = SSL_get_error(datahandle->ssl, success);
		perr = ERR_error_string(failed, NULL);
		printf("ssl connect err:[%s] errcode:[%d]\n",perr,failed);
		return -1;	
	} 	
	
	return 0;
}
 
/**
开启被动模式的数据连接
**/
int FtpsOpenPasvDataConn(FtpsClient *cli,FtpsClient *datahandle,const char *cmd)
{
	if(cli == NULL || datahandle == NULL || cmd == NULL){
		printf("cli or datahandle or cmd is null pointer");
		return -1;
	}
	
	int failed = 0;
	char buff[1024];
	bzero(buff,sizeof(buff));
 
	failed = FtpsOpenPasvSockConn(cli,datahandle);
	if(failed){
		return -1;	
	}
	
	EnSend(cli,TYPE_I_CMD,strlen(TYPE_I_CMD));
//	printf("after send TYPE_I_CMD \r\n");
	PrintResp(cli);
	
	EnSend(cli,cmd,strlen(cmd));/** 只有发送带数据流的命令才能SSL连接 **/
//	printf("after send cmd  recv %s\r\n",buff);
	EnRecv(cli,buff,sizeof(buff));
	
	if(buff[0] == '5'){
		printf("Failed to execute %s \n",cmd);
		printf("%s\n",buff);
		close(datahandle->connfd);
		return -1;
	}
	
	failed = FtpsOpenPasvSslConn(cli,datahandle);
	if(failed){
		close(datahandle->connfd);
		return -1;	
	}
	
	return 0;
}
 
int FtpsAccessDataByPasvMode(FtpsClient *cli,const char *localpath,const char *cmd)
{
	if(cli == NULL || cmd ==NULL)
		return -1;
 
	if(localpath == NULL){
			localpath = "./temp.record";
	}
 
	int failed = 0,len = 0;
	FtpsClient *datahandle = NULL;
	FILE *fp = NULL;
	char buff[1024];
	
	bzero(buff,sizeof(buff));
	
	datahandle = (FtpsClient *)malloc(sizeof(FtpsClient) + 1);
	if(datahandle == NULL){
		perror("datahandle malloc err:");	
		return -1;
	}
//	printf("FtpsAccessDataByPasvMode datahandle size = %d addr %p\r\n", sizeof(FtpsClient), datahandle);
	bzero(datahandle,sizeof(FtpsClient));
	
	fp = fopen(localpath,"wb+");
	if(fp == NULL){
		perror("To open local path error when download file:");	
		return -1;
	}
	printf("localpath %s open\r\n", localpath);
	failed = FtpsOpenPasvDataConn(cli,datahandle,cmd);
	if(failed){
		printf("Ftps Open Pasv Data Connection error\n");
		return -1;
	}
	
	while( (len = SSL_read(datahandle->ssl,buff,sizeof(buff))) > 0){
		if (fwrite(buff, 1, len, fp) <= 0)
		{
			perror("local path");
			break;
		}		
		bzero(buff,sizeof(buff));
	}//end while	
 
	
	CloseFtpsDataConn(cli,datahandle);
	fclose(fp);
	return 0;	
	
}


int FtpsUploadDataByPasvMode(FtpsClient *cli,const char *localpath, const char *cmd)
{
	if(cli == NULL || cmd ==NULL)
		return -1;
 
	if(localpath == NULL){
			localpath = "./temp.record";
	}
 
	int failed = 0,len = 0;
	FtpsClient *datahandle = NULL;
	FILE *fp = NULL;
	char buff[1024];
	
	bzero(buff,sizeof(buff));
	
	datahandle = (FtpsClient *)malloc(sizeof(FtpsClient) + 1);
	if(datahandle == NULL){
		perror("datahandle malloc err:");	
		return -1;
	}
//	printf("FtpsUploadDataByPasvMode datahandle size = %d addr %p\r\n", sizeof(FtpsClient), datahandle);
	bzero(datahandle,sizeof(FtpsClient));
	
	fp = fopen(localpath,"rb");
	if(fp == NULL){
		perror("To open local path error when download file:");	
		return -1;
	}
	printf("localpath %s open\r\n", localpath);
	failed = FtpsOpenPasvDataConn(cli,datahandle,cmd);
	if(failed){
		printf("Ftps Open Pasv Data Connection error\n");
		return -1;
	}
	
	while((len = fread(buff, 1, 1024, fp)) > 0){
		int ret = SSL_write(datahandle->ssl, buff, len);
		if(ret <= 0){
			printf("SSL_write error\r\n");
			break;
		}
		bzero(buff,sizeof(buff));
	}

	CloseFtpsDataConn(cli,datahandle);
	fclose(fp);
	return 0;	
	
}


 
/**
下载文件
**/
int FtpsGet(FtpsClient *cli,const char *localpath,const char *remotepath)
{
	char cmd[256];
	int failed = 0;
//	char buff[1024];
	
	bzero(cmd,sizeof(cmd));
//	bzero(buff,sizeof(buff));
 
 	printf("start FtpsGet\r\n");
	snprintf(cmd,sizeof(cmd),"RETR %s\r\n",remotepath);
	
	failed = FtpsAccessDataByPasvMode(cli, localpath, cmd);
	if(failed){
		printf("Download remote File[%s] failed\n",cmd);
		return -1;
	}
	
	return 0;	
}

/*
	上传文件
*/

int FtpsPut(FtpsClient *cli, const char * remotepath, const char * localpath)
{
	char cmd[256] = {0};
	int failed = 0;

	printf("start FtpsPut\r\n");
	snprintf(cmd, sizeof(cmd), "STOR %s\r\n", remotepath);

	failed = FtpsUploadDataByPasvMode(cli, localpath, cmd);
	if(failed){
		printf("Upload local File[%s] failed\n",cmd);
		return -1;
	}
	
	return 0;
}

/**
获取路径下的文件大小
**/
int FtpsSize(FtpsClient *cli,const char *path, size_t *size)
{
	char cmd[256],buff[1024];
	size_t resp = 0,sz = 0,rv=0;
	bzero(cmd,sizeof(cmd));
	bzero(buff,sizeof(buff));
 
	snprintf(cmd,sizeof(cmd), TYPE_A_CMD);
	EnSend(cli,cmd,strlen(cmd));
	EnRecv(cli,buff,sizeof(buff));
	printf("recv = %s\n",buff);
	
	bzero(buff,sizeof(buff));
	snprintf(cmd,sizeof(cmd),"SIZE %s\r\n",path);
	EnSend(cli,cmd,strlen(cmd));
	EnRecv(cli,buff,sizeof(buff));
	printf("recv = %s\n",buff);
 
	if (sscanf(buff, "%lu %lu", &resp, &sz) == 2) 
		*size = sz;
	else 
		rv = -1;
 
   return rv;
}
 
 
/**
遍历目录
**/
int FtpsDir(FtpsClient *cli,const char *localpath,const char *remotepath)
{
	char cmd[1024];
	int failed = 0;
 
	
	bzero(cmd,sizeof(cmd));
 
	snprintf(cmd,sizeof(cmd),"LIST -aL %s\r\n",remotepath);
	
	failed = FtpsAccessDataByPasvMode(cli, localpath, cmd);
	if(failed){
		printf("Download remote File[%s] failed\n",cmd);
		return -1;
	}
	
	return 0;		
}
 
/**
切换目录
**/
int FtpsChdir(FtpsClient *cli,const char *path)
{
	if(path == NULL)
		return 0;
	
	char buf[256];
	int ret = 0;
	
	sprintf(buf,"CWD %s\r\n",path);
	ret = EnSend(cli,buf,strlen(buf));
	if(ret <0){
		return -1;
	}	
	
	bzero(buf,sizeof(buf));
	ret = EnRecv(cli,buf,sizeof(buf));
	if(ret <=0){
		return -1;
	}	
	printf("%s\n",buf);
	if(buf[0] == '5')
		return -1;	
	
	return 0;
}
 
 
/**
关闭socket及SSL连接
**/
int CloseFtpsClient(FtpsClient *cli)
{
	if(cli == NULL)
		return 0;
	if(cli->ssl != NULL)
		SSL_free(cli->ssl);
	if(cli->ctx != NULL)	
		SSL_CTX_free(cli->ctx);	
	close(cli->connfd);
	free(cli);
	
	return 0;
} 
 
 
int CloseFtpsDataConn(FtpsClient *cli,FtpsClient *datahandle)
{
	printf("enter CloseFtpsDataConn \r\n");
	EnSend(cli,QUIT_CMD,strlen(QUIT_CMD));
	CloseFtpsClient(datahandle);
	return 0;
}
 
 
/**==================================================FTP+SSL/TLS or FTPS code segment end==================================================**/
//		 0      1      2         3      4   5    6    7
//  ./ftps_app get localpath remotepath ip port user pass
int main(int argc, char * argv[])
{
	if(argc != 8){
		printf("Usage :\r\n");
		printf("./ftps_app get localpath remotepath ip port user pass \r\n");
		printf("./ftps_app put remotepath localpath ip port user pass \r\n");
		return -1;
	}

	char host[24] = {0}; //10.106.249.31:30021
	int failed =  0;
	
	sprintf(host, "%s:%s", argv[4], argv[5]);
	
	FtpsClient * cli = OpenFtpsClient(host);
	if(cli == NULL)
		return -1;
		
	failed = FtpsLogin(cli, argv[6], argv[7]);	
	if(failed)
		return -1;
 
	if(strcmp(argv[1], "get") == 0){	
		failed = FtpsGet(cli, argv[2], argv[3]);	
		if(failed)
			return -1;	
	}
	else if(strcmp(argv[1], "put") == 0){
		failed = FtpsPut(cli, argv[2], argv[3]);
		if(failed)
				return -1;	
	}
	
	CloseFtpsClient(cli);
	
	return 0;
}
 
 
