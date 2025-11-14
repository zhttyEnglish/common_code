#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "cJSON.h"
/*
1.2辅控APP通知主站协议
以“{app1}/terminal/dataReport/{app1}/{app1}/{app1}/{app1}/ TopoFile”主题playload通知IotAPP。
*/
int bulid_json_app_nitify_host(char * buf, char * token, char * timestamp, char * serviceid, 
												char * dir, char * filename, char * filetype, char * compress)
{
	cJSON *root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "token", token);
	cJSON_AddStringToObject(root, "timestamp", timestamp);

	cJSON *body = cJSON_CreateObject();
	cJSON_AddStringToObject(body, "serviceid", serviceid);
	
	cJSON *serviceProperties = cJSON_CreateObject();
	cJSON_AddStringToObject(serviceProperties, "Dir", dir);
	cJSON_AddStringToObject(serviceProperties, "filename", filename);
	cJSON_AddStringToObject(serviceProperties, "fileType", filetype);
	cJSON_AddStringToObject(serviceProperties, "compress", compress);

	cJSON_AddItemToObject(body, "serviceProperties", serviceProperties);

	cJSON_AddItemToObject(root, "body", body);
	
	char * str = cJSON_Print(root);
    //log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	
	return 0;
}

/*
3、IotAPP通知主站接口： 
/v1/file/${gatewayId}/add
*/
int build_json_IOT_notifi_host(char * buf, char * deviceid, char * serviceid, char * eventTime, int expire, 
											char * name, char * addr, int size, char * md5, char * fileType, char * compress)
{
	cJSON * root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "deviceId", deviceid);
	cJSON_AddStringToObject(root, "serviceId", serviceid);
	cJSON_AddStringToObject(root, "eventTime", eventTime);
	cJSON_AddNumberToObject(root, "expire", expire);

	cJSON * data = cJSON_CreateObject();
	cJSON_AddStringToObject(data, "name", name);
	cJSON_AddStringToObject(data, "addr", addr);
	cJSON_AddNumberToObject(data, "size", size);
	cJSON_AddStringToObject(data, "md5", md5);
	cJSON_AddStringToObject(data, "fileType", fileType);
	cJSON_AddStringToObject(data, "compress", compress);

	cJSON_AddItemToObject(root, "data", data);
	
	char * str = cJSON_Print(root);
    //log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}

/*
1. 主站下发指令IotApp 
Topic：/v1/devices/{gatewayId}/command  (平台发起，终端接收)
*/
int build_json_host_to_IOT(char * buf, char * msg_type, int mid, char * cmd, 
									char * param, char * serviceid, char * deviceid)
{
	cJSON * root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "msgType", msg_type);
	cJSON_AddNumberToObject(root, "mid", mid);
	cJSON_AddStringToObject(root, "cmd", cmd);

	cJSON * paras = cJSON_CreateObject();
	cJSON_AddStringToObject(paras, "A_phsC", param);

	cJSON_AddItemToObject(root, "paras", paras);
	cJSON_AddStringToObject(root, "deviceId", deviceid);
	cJSON_AddStringToObject(root, "serviceId", serviceid);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}

/*
2. IotApp下发召测命令给辅控系统APP协议(Iot发起，辅控app接收)
esdk/terminal/cmdReq/@manufacturerId/@manufacturerName/@deviceType/@model/@nodeId 

字段					(长度)			值定义
manufacturerId		(string 32)		终端厂家ID
manufacturerName	(string 32)		厂家名称
deviceType			(string 32)		终端类型
model				(string 32)		终端型号
nodeId				(string 32)		端设备序列号，必须唯一(GUID:每个相机唯一)
requestId			(string 48)		请求ID,在响应消息中需要使用
serviceId			(string 32)		服务ID
method				(string 32)		命令字
cmdContent			(string 10240)	命令内容
*/
int build_json_IOT_to_app(char * buf, char * requestId, char * serviceId, 
									char * method, char * pic_id, char * token, char * timestamp)
{
	cJSON * root = cJSON_CreateObject();
	
	cJSON * body = cJSON_CreateObject();
	cJSON_AddStringToObject(body, "requestId", requestId);
	cJSON_AddStringToObject(body, "serviceId", serviceId);
	cJSON_AddStringToObject(body, "method", method);

	cJSON * cmdContent = cJSON_CreateObject();
	cJSON_AddStringToObject(cmdContent, "pic_id", pic_id);
	
	cJSON_AddItemToObject(body, "cmdContent", cmdContent);

	cJSON_AddItemToObject(root, "body", body);
	
	cJSON_AddStringToObject(root, "token", token);
	cJSON_AddStringToObject(root, "timestamp", timestamp);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}

/*
3. 辅控系统APP响应IotApp的召测命令(辅控App发起, IotApp接收)
@app/terminal/cmdRsp/@manufacturerId/@manufacturerName/@deviceType/@model/@nodeId	  

字段					(长度)			值定义
app					(string 32)		app名称为各自业务APP自己定义名称，注意名字不能为esdk
manufacturerId		(string 32)		终端厂家ID
manufacturerName	(string 32) 	厂家名称
deviceType			(string 32)		终端类型
model				(string 32)		终端型号
nodeId				(string 32)		端设备序列号，必须唯一
requestId			(string 48)		请求ID,在响应消息中需要使用
serviceId			(string 32)		服务ID
serviceProperties	(string 10240) 	服务属性，就是要具体上报的数据信息
*/
int build_json_app_to_IOT(char * buf, char * requestId, char * serviceId, 
									char * pic_id, char * token, char * timestamp)
{
	cJSON * root = cJSON_CreateObject();
	
	cJSON * body = cJSON_CreateObject();
	cJSON_AddStringToObject(body, "requestId", requestId);
	cJSON_AddStringToObject(body, "serviceId", serviceId);

	cJSON * serviceProperties = cJSON_CreateObject();
	cJSON_AddStringToObject(serviceProperties, "pic_id", pic_id);
	
	cJSON_AddItemToObject(body, "serviceProperties", serviceProperties);

	cJSON_AddItemToObject(root, "body", body);
	
	cJSON_AddStringToObject(root, "token", token);
	cJSON_AddStringToObject(root, "timestamp", timestamp);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}


/*
云边交互示例：
Topic：/v1/devices/{gatewayId}/command  (平台发起，终端接收)
*/
int cloud_edge_platform_to_terminal(char * buf, char * msg_type, int mid, char * cmd, 
											int remote_permit, char * serviceid, char * deviceid)
{
	cJSON * root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "msgType", msg_type);
	cJSON_AddNumberToObject(root, "mid", mid);
	cJSON_AddStringToObject(root, "cmd", cmd);

	cJSON * paras = cJSON_CreateObject();
	cJSON_AddNumberToObject(paras, "remote_permit", remote_permit);

	cJSON_AddItemToObject(root, "paras", paras);
	cJSON_AddStringToObject(root, "deviceId", deviceid);
	cJSON_AddStringToObject(root, "serviceId", serviceid);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}
/*
云边交互示例：
Topic：/v1/devices/{gatewayId}/commandResponse  (终端发起，平台接收)
*/
int cloud_edge_terminal_to_platform(char * buf, char * msg_type, int mid, int errcode, int remote_permit)
{
	cJSON * root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "msgType", msg_type);
	cJSON_AddNumberToObject(root, "mid", mid);
	cJSON_AddNumberToObject(root, "errcode", errcode);

	cJSON * body = cJSON_CreateObject();
	cJSON_AddNumberToObject(body, "remote_permit", remote_permit);

	cJSON_AddItemToObject(root, "body", body);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);

	memcpy(buf, str, strlen(str));
	return 0;
}


/*
3.边端交互示例：
Topic说明：
esdk/terminal/cmdReq/@manufacturerId/@manufacturerName/@deviceType/@model/@nodeId   
字段					(长度)			值定义
manufacturerId		(string 32)		终端厂家ID
manufacturerName	(string 32)		厂家名称
deviceType			(string 32)		终端类型
model				(string 32)		终端型号
nodeId				(string 32)		端设备序列号，必须唯一
requestId			(string 48)		请求ID,在响应消息中需要使用
serviceId			(string 32)		服务ID
method				(string 32)		命令字
cmdContent			(string 10240)	命令内容
*/
int build_json_edge_interaction(char * buf, char * requestid, char * serviceid, char * method, 
											int remote_permit, char * token, char * timestamp)
{
	cJSON * root = cJSON_CreateObject();
	
	cJSON * body = cJSON_CreateObject();
	cJSON_AddStringToObject(body, "requestId", requestid);
	cJSON_AddStringToObject(body, "serviceId", serviceid);
	cJSON_AddStringToObject(body, "method", method);

	cJSON * cmdContent = cJSON_CreateObject();
	cJSON_AddNumberToObject(cmdContent, "remote_permit", remote_permit);

	cJSON_AddItemToObject(body, "cmdContent", cmdContent);
	cJSON_AddItemToObject(root, "body", body);
	cJSON_AddStringToObject(root, "token", token);
	cJSON_AddStringToObject(root, "timestamp", timestamp);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);
	memcpy(buf, str, strlen(str));
	
	return 0;
}

/*
定值设置：{app}/set/request/database/parameter
*/
int build_json_set_constant_value(char * buf, char * token, char * timestamp, char * device, 
												char name[][24], char value[][8], char dataType[][8], int len)
{
	cJSON * root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "token", token);
	cJSON_AddStringToObject(root, "timestamp", timestamp);

	cJSON * body_outer = cJSON_CreateArray();
	cJSON * body_outer_item = cJSON_CreateObject();
	cJSON_AddStringToObject(body_outer_item, "dev", device);

	cJSON * body_inner = cJSON_CreateArray();	
	for(int i = 0; i < len; i ++)
	{
		cJSON* body_inner_item = cJSON_CreateObject();
	    cJSON_AddStringToObject(body_inner_item, "name", name[i]);
	    cJSON_AddStringToObject(body_inner_item, "val", value[i]);
		cJSON_AddStringToObject(body_inner_item, "dataType", dataType[i]);
	    cJSON_AddItemToArray(body_inner, body_inner_item);
	}
	cJSON_AddItemToObject(body_outer_item, "body", body_inner);
	cJSON_AddItemToArray(body_outer, body_outer_item);
	
	cJSON_AddItemToObject(root, "body", body_outer);

	char * str = cJSON_Print(root);
	//log("\r\n%s\r\n", str);
	memcpy(buf, str, strlen(str));
	
	return 0;
}

