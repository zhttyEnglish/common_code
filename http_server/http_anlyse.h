#ifndef _HTTP_H_
#define _HTTP_H_

struct multi_str{
	int count;
	char data[8][8];
};

/*
 *   ��������			   ����	  ��ѡ/��ѡ				��������						��ע
 * requestHostIp	  string	��ѡ			�����������ip��ַ	
 * requestHostPort	  string	��ѡ			����������ض˿�	
 * requestId		  string	��ѡ			�����������Ψһ��ʶ,UUID	
 * objectList		  string	��ѡ			�����б�,JSON��ʽ������	
 * objectId			  string	��ѡ			������λ��ʶ       				  Ѳ�ӵ�λID
 * typeList			  string	��ѡ			ͼ���������	
 * imageNormalPath	  string	��ѡ			�б��׼ͼ						  �����б�ģ��
 * imagePathList	  string	��ѡ			������ͼ���·��	
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
 *	��������		 ����		��ѡ/��ѡ		��������					��ע
 * requestId	string	  ��ѡ		����ʶ��Ψһ��ʶ��UUID	
 * resultsList	string	  ��ѡ		�����	
 * objectId		string	  ��ѡ		������λ��ʶ					Ѳ�ӵ�λ ID
 * results		string	  ��ѡ		�������	
 * type			string	  ��ѡ		��������	������� N.3.6
 * value		string	  ��ѡ		ֵ	������� N.3.6
 * code			string	  ��ѡ		��ȷ 2000 ͼ�����ݴ��� 2001 �㷨����ʧ�� 2002	ͼ�����ݴ�����ָδ�� ��ȡ��ͼ�����ݣ��㷨 ����ʧ����ָ�㷨���� ���������г���
 * resImageUrl	string	  ��ѡ		�������ͼ�� url ·��	
 * pos			string	  ��ѡ		ͼ������,����˳ʱ��˳���ṩ��һϵ�������б�,����"areas":[{"x":"10","y":"15"},{"x":"100"," y":"150"},{"x":"100","y":"500"},{"x":"10","y":"500"}]	
 * conf  		float	  ��ѡ		����������Ŷ�	��Χ 0-1������ 4 λС��
 * desc			string	  ��ѡ		�����������	
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
 *	��������			����		  ��ѡ/��ѡ			��������				��ע
 *	requestId		string		��ѡ			�����������Ψһ�� ʶ��UUID	
 *	objectList		string		��ѡ			�����ֶ�����	
 *	taskId			Long	 	��ѡ			����id	
 *	taskInsId		Long		��ѡ			����ʵ��id	
 *	taskNodeId		Long		��ѡ			��λid	
 *	beuId			Long		��ѡ			�����id	
 *	beuName			string		��ѡ			���������	
 *	type			string		��ѡ			�㷨ʶ�����	������� N.3.6
 *	algName			string		��ѡ			�㷨����	
 *	subtypeList		string		��ѡ			�㷨ʶ��С��	
 *	imageUrlList	string		��ѡ			ʶ��ͼ��·��	
 *	imageNormalUrlPath	string	��ѡ			�б��׼ͼ	
 *	actionType		Integer		��ѡ			ģʽ	1����ģʽ��2׷��ģʽ
 *	mspId			Integer		��ѡ			����DT��id	
 *	users			string		��ѡ			�û�	
 *	infraredArea	string		��ѡ			��������	4096,4880;5542,4880;5542,1728;4096,1728"
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
 * ��������			  		����		��ѡ/��ѡ		��������				��ע
 * taskId		  		Long	 ��ѡ			����id	
 * taskInsId	 		Long	 ��ѡ			ִ��ʵ��id	
 * taskNodeId	 		Long	 ��ѡ			�����λid	
 * beuId		  		Long	 ��ѡ			�����id	
 * actionType	  		Integer	 ��ѡ			ģʽ	1����ģʽ��2׷��ģʽ
 * results		  		string	 ��ѡ			���	
 * algName		  		string	 ��ѡ		
 * detectType	  		string	 ��ѡ			ʶ�����	������� N.3.6
 * detectSubType  		string	 ��ѡ			ʶ��С��	
 * handleFilePath 		string	 ��ѡ			�㷨������ͼƬ	
 * srcFilePath	  		string	 ��ѡ			ԭͼ	
 * handelVideoFilePath	string	 ��ѡ			�㷨��������Ƶ����ַ	
 * result		  		string	 ��ѡ			ʶ����	������� N.3.6
 * resCode		  		Integer	 ��ѡ			��ȷ 2000 ͼ�����ݴ��� 2001 �㷨����ʧ�� 2002	
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
	int isolator;  // ��բ״̬ 1:��  2:��  3:���쳣 4:���쳣
	int switch_status;    // ����״̬ 1:�� 2:�� 0:Ԥ��
	int meter;     // �Ǳ���� �ɷ��ض��ֵ �Զ��Ÿ���
	int infrared[2];  // �����¶� ��������� ����� �Զ��Ÿ���
	int sound;	   // ����   1:����   2:�쳣
	int light;	   // ָʾ��		1 �� 2 �� 3 �̵Ƴ��� 4 ��Ƴ��� 5 �̵���˸ 6 �����˸
	char  qrcode[32]; // ʵ��id
}device_status;

typedef struct device_defect{
	int tx_pb;		//ȱ���б�	0	 ��ȱ��	1 ��ȱ��
	int tx_yzwpy;   //Ԥ��λƫ��	0	��ƫ��	1 ��ƫ��
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


/* socket_bind_listen	����socket,��,����
 * return 	���� 	sockfd 
 */
int socket_bind_listen();

/* parse_callback_packet  ������Ӧ��
*/
int parse_callback_packet(char * data);

/* build_error_callback  ������Ӧ��
*/
int build_error_callback(char * buf, int error_type);

/* build_callback_packet �������ذ�
*/
int build_callback_packet(char * buf, char * content, int content_len, char * file_name);

/* build_B_callback_content  ����A���㷨����ص�post����
 * buf		���		�����ı�����buf��
 * return	����		���ر��ĵĳ���
 */
int build_B_callback_content(char * buf);

/* parse_post_B_content  ����B���㷨post���ĵ�content
 * data  ����  	���ĵ�content����
 */
int parse_post_B_content(char * data);

/* build_A_callback_content  ����A���㷨����ص�post����
 * buf 		���		�����ı�����buf��
 * return	����      ���ر��ĵĳ���
 */
int build_A_callback_content(char * buf);

/* parse_post_A_content  ����A���㷨post���ĵ�content
 * data  ����  	���ĵ�content����
 */
int parse_post_A_content(char * data);

/* check_if_post    ����Ƿ�Ϊpost����
 *
 * data   	    ����      ��������
 * file_name    ��� 	    ��Ҫ�������ļ���
 * content	    ���	    ���İ���������
 * return       ����    	����post ����-1  	��A���㷨 ���� 0  		B���㷨����	1      callback ����    2
 */
int check_if_post(char * data, char * file_name, char * content);
int tcp_connect(char * request_ip, int request_post);


#endif

