int bulid_json_app_nitify_host(char * buf, char * token, char * timestamp, char * serviceid, 
										char * dir, char * filename, char * filetype, char * compress);

int build_json_IOT_notifi_host(char * buf, char * deviceid, char * serviceid, char * eventTime, int expire, 
										char * name, char * addr, int size, char * md5, char * fileType, char * compress);

int build_json_host_to_IOT(char * buf, char * msg_type, int mid, char * cmd, 
									char * param, char * serviceid, char * deviceid);

int build_json_IOT_to_app(char * buf, char * requestId, char * serviceId, 
									char * method, char * pic_id, char * token, char * timestamp);

int build_json_app_to_IOT(char * buf, char * requestId, char * serviceId, 
									char * pic_id, char * token, char * timestamp);

int cloud_edge_platform_to_terminal(char * buf, char * msg_type, int mid, char * cmd, 
											int remote_permit, char * serviceid, char * deviceid);

int cloud_edge_terminal_to_platform(char * buf, char * msg_type, int mid, int errcode, int remote_permit);

int build_json_edge_interaction(char * buf, char * requestid, char * serviceid, char * method, 
											int remote_permit, char * token, char * timestamp);

int build_json_set_constant_value(char * buf, char * token, char * timestamp, char * device, 
												char name[][24], char value[][8], char dataType[][8], int len);


