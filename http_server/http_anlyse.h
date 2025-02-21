#ifndef _HTTP_H_
#define _HTTP_H_

struct multi_str{
	int count;
	char data[8][8];
};

/*
 *   参数名称			   类型	  必选/可选				参数含义						备注
 * requestHostIp	  string	必选			分析结果反馈ip地址	
 * requestHostPort	  string	必选			分析结果返回端口	
 * requestId		  string	必选			请求分析数据唯一标识,UUID	
 * objectList		  string	必选			分析列表,JSON格式的数组	
 * objectId			  string	必选			分析点位标识       				  巡视点位ID
 * typeList			  string	必选			图像分析类型	
 * imageNormalPath	  string	可选			判别基准图						  用于判别模板
 * imagePathList	  string	必选			待分析图像的路径	
 */
typedef struct request_content_A_t
{
	char  request_host_ip[16];
	int  request_host_port;
	char  request_id[64];
	char  object_list[512];
	int  object_id;
	char  type_list[32];
	struct multi_str type_data;
	char  image_normal_path[128];//
	struct multi_str image_normal_path_data;
	char  image_path_list[128];
	struct multi_str image_path_list_data;
}
request_content_A_t;

typedef request_content_A_t * request_content_A_handle_t;

/*
 *	参数名称		 类型		必选/可选		参数含义					备注
 * requestId	string	  必选		请求识别唯一标识，UUID	
 * resultsList	string	  必选		结果集	
 * objectId		string	  必选		分析点位标识					巡视点位 ID
 * results		string	  必选		分析结果	
 * type			string	  必选		分析类型	含义详见 N.3.6
 * value		string	  必选		值	含义详见 N.3.6
 * code			string	  必选		正确 2000 图像数据错误 2001 算法分析失败 2002	图像数据错误是指未能 获取到图像数据；算法 分析失败是指算法本身 分析过程中出错
 * resImageUrl	string	  必选		结果反馈图像 url 路径	
 * pos			string	  必选		图中区域,按照顺时针顺序提供的一系列坐标列表,例如"areas":[{"x":"10","y":"15"},{"x":"100"," y":"150"},{"x":"100","y":"500"},{"x":"10","y":"500"}]	
 * conf  		float	  必选		分析结果置信度	范围 0-1，保留 4 位小数
 * desc			string	  必选		分析结果描述	
 */
typedef struct response_content_A_t
{
	float conf;
	int area_num;
	int point_num[8];
	int pos_x[8][8];
	int pos_y[8][8];
	char  request_id[64];
//	char  result_list[32];
	int  object_id;
//	char  result[32];
	char  type[32];
	int  value;
	int  code;
	char  res_iamge_url[32];
	char  desc[32];
}
response_content_A_t;

typedef response_content_A_t * response_content_A_handle_t;

struct Area{
	int count;
	double  x[8];
	double  y[8];
};

#if 0
/*
 *	参数名称			类型		  必选/可选			参数含义				备注
 *	requestId		string		必选			请求分析数据唯一标 识，UUID	
 *	objectList		string		必选			请求字段内容	
 *	taskId			Long	 	必选			任务id	
 *	taskInsId		Long		必选			任务实例id	
 *	taskNodeId		Long		必选			点位id	
 *	beuId			Long		必选			任务点id	
 *	beuName			string		可选			任务点名称	
 *	type			string		必选			算法识别大类	含义详见 N.3.6
 *	algName			string		可选			算法名称	
 *	subtypeList		string		必选			算法识别小类	
 *	imageUrlList	string		必选			识别图像路径	
 *	imageNormalUrlPath	string	可选			判别基准图	
 *	actionType		Integer		必选			模式	1覆盖模式，2追加模式
 *	mspId			Integer		可选			所属DT包id	
 *	users			string		可选			用户	
 *	infraredArea	string		可选			红外区域	4096,4880;5542,4880;5542,1728;4096,1728"
 */
typedef struct request_content_B_t
{
	int		action_type;
	int 	msp_id;//
	long	task_id;
	long	task_ins_id;
	long	task_node_id;
	long	beu_id;
	char 	request_id[64];
	char 	object_list[32];
	char 	beu_name[32];//
	char 	type[32];
	struct multi_str type_data;
	char 	alg_name[32];//
	char 	sub_type_list[32];
	struct multi_str sub_type_list_data;
	char 	image_url_list[32];
	struct multi_str image_url_list_data;
	char 	image_normal_url_path[32];//
	char 	users[32];//
	char 	infrared_area[128];//
	struct infraredArea infrared_area_data;
}
request_content_B_t;

typedef request_content_B_t * request_content_B_handle_t;

struct detectType{
	int count;
	char  type[4][8];
};

/*
 * 参数名称			  		类型		必选/可选		参数含义				备注
 * taskId		  		Long	 必选			任务id	
 * taskInsId	 		Long	 必选			执行实例id	
 * taskNodeId	 		Long	 必选			任务点位id	
 * beuId		  		Long	 必选			任务点id	
 * actionType	  		Integer	 必选			模式	1覆盖模式，2追加模式
 * results		  		string	 必选			结果	
 * algName		  		string	 可选		
 * detectType	  		string	 必选			识别大类	含义详见 N.3.6
 * detectSubType  		string	 必选			识别小类	
 * handleFilePath 		string	 必选			算法处理后的图片	
 * srcFilePath	  		string	 必选			原图	
 * handelVideoFilePath	string	 可选			算法处理后的视频流地址	
 * result		  		string	 必选			识别结果	含义详见 N.3.6
 * resCode		  		Integer	 必选			正确 2000 图像数据错误 2001 算法分析失败 2002	
 */
typedef struct response_content_B_t
{
	int 	action_type;
	int 	res_code;
	long 	task_id;
	long 	task_ins_id;
	long 	task_node_id;
	long 	beu_id;
	char 	result[32];
	char 	alg_name[32];
	struct detectType	detect_type;
	struct detectType	detect_sub_type;
	char 	handle_file_path[32];
	char 	src_file_path[32];
	char 	handle_video_file_path[32];
}
response_content_B_t;

typedef response_content_B_t * response_content_B_handle_t;
#endif
typedef struct result_json_t{
	double x[8];
	double y[8];
	double w[8];
	double h[8];
	double confidence[8];
	int cls_id[8];
	int count;
	char type[32];
	int value;
	int code;
	char res_image_path[128];
	char desc[32];
	struct Area area[4];
	double conf;
}result_json_t;

typedef result_json_t * result_json_handle_t;

typedef struct device_status{
	int isolator;  // 刀闸状态 1:分  2:合  3:分异常 4:合异常
	int switch_status;    // 开关状态 1:分 2:合 0:预留
	int meter;     // 仪表读数 可返回多个值 以逗号隔开
	int infrared[2];  // 红外温度 返回最高温 最低温 以逗号隔开
	int sound;	   // 声音   1:正常   2:异常
	int light;	   // 指示灯		1 灭 2 亮 3 绿灯常亮 4 红灯常亮 5 绿灯闪烁 6 红灯闪烁
	char  qrcode[32]; // 实物id
}device_status;

typedef struct device_defect{
	int tx_pb;		//缺陷判别	0	 无缺陷	1 有缺陷
	int tx_yzwpy;   //预置位偏移	0	无偏移	1 有偏移
}device_defect;

typedef enum defect_type{
	sly_bjbmyw_cysb_cyg = 1,
	sly_bjbmyw_cysb_sgz = 2, 
	sly_bjbmyw_cysb_tg = 3,
	sly_bjbmyw_cysb_lqq = 4,
	sly_bjbmyw_cysb_qyb = 5,
	sly_bjbmyw_cysb_qtjdq = 6,
	sly_bjbmyw_ylsff  = 7,
	sly_dmyw = 8,
	pzqcd = 9,
	jyhbx = 10,
	drqgd = 11,
	jyz_pl = 12,
	yxdgsg = 13,
	jdyxxsd = 14,
	jsxs_ddjt = 15,
	jsxs_ddyx = 16,
	jsxs_jdyxx = 17,
	jsxs_ecjxh = 18,
	hzyw = 19,
	bmwh = 20,
	
	ws_ywyc = 21,
	ws_ywzc = 22,//
	yljdq_flow = 23,
	yljdq_stop = 24,
	fhz_f = 25,
	fhz_h = 26,
	fhz_ztyc = 27,
	hxq_gjbs = 24,
	hxq_gjzc = 28,//
	ywzt_yfyc = 29,
	ywzr_yfzc = 30,//

	hxq_gjtps = 31,
	hxq_yfps = 32,

	bjzc = 33,//
	bj_wkps = 34,
	bj_bpps = 35,

	bjdsyc_zz = 36,
	bjdsyc_sx = 37,
	SF6ylb = 38,
	xldlb = 39,
	ylb = 40,
	ywb = 41,
	
	bj_bpmh = 42,
	bjdsyc_ywj = 43,
	bjdsyc_ywc = 44,
	gcc_mh = 45,
	gcc_ps = 46,
	
	xmbhyc = 47,
	xmbhzc = 48,//

	yw_nc = 49,
	yw_gkxfw = 50,

	kgg_ybf = 51,
	kgg_ybh = 52,
	kk_f = 53,
	kk_h = 54,
	zsd_l = 55,
	zsd_m = 56,

	mbhp = 57,

	wcgz = 58,
	gzzc = 59,//

	wcaqm = 60,
	aqmzc = 61,//
	xy = 62,
}defect_type;


/* socket_bind_listen	创建socket,绑定,监听
 * return 	返回 	sockfd 
 */
int socket_bind_listen();

/* parse_callback_packet  解析响应包
*/
int parse_callback_packet(char * data);

/* build_error_callback  构建响应包
*/
int build_error_callback(char * buf, int error_type);

/* build_callback_packet 构建返回包
*/
int build_callback_packet(char * buf, char * content, int content_len, char * file_name);

/* build_B_callback_content  构建A类算法结果回调post报文
 * buf		输出		将报文保存在buf里
 * return	返回		返回报文的长度
 */
int build_B_callback_content(char * buf);

/* parse_post_B_content  解析B类算法post报文的content
 * data  输入  	报文的content数据
 */
int parse_post_B_content(char * data);

/* build_A_callback_content  构建A类算法结果回调post报文
 * buf 		输出		将报文保存在buf里
 * return	返回      返回报文的长度
 */
int build_A_callback_content(char * buf);

/* parse_post_A_content  解析A类算法post报文的content
 * data  输入  	报文的content数据
 */
int parse_post_A_content(char * data);

/* check_if_post    检查是否为post请求
 *
 * data   	    输入      报文数据
 * file_name    输出 	    需要操作的文件名
 * content	    输出	    报文包含的内容
 * return       返回    	不是post 返回-1  	是A类算法 返回 0  		B类算法返回	1      callback 返回    2
 */
int check_if_post(char * data, char * file_name, char * content);
int tcp_connect(char * request_ip, int request_post);


#endif

