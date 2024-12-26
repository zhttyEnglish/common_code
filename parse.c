/* 构建 json 数据包 */
pos += sprintf(&buf[pos], "{");
pos += sprintf(&buf[pos], "\"cmd\":\"HANG_UP\",");
pos += sprintf(&buf[pos], "\"data\":{");
pos += sprintf(&buf[pos], "\"channelName\":\"%s\"", request_json.data.channel_name);
pos += sprintf(&buf[pos], "},");
pos += sprintf(&buf[pos], "\"device\":\"%s\",", request_json.device);
pos += sprintf(&buf[pos], "\"requestId\":\"%s\",", request_json.request_id);
pos += sprintf(&buf[pos], "\"version\":\"%s\"", request_json.version);
pos += sprintf(&buf[pos], "}");

/* 解析 json 数据 */
// 包含在 { }  里的数据 , valueEnd要修改
char * code_Start = NULL;
char * code_ValueStart = NULL;
char * code_ValueEnd = NULL;
code_Start = strstr(json, "\"code\":");
if(code_Start != NULL){
    code_ValueStart = strchr(code_Start + strlen("\"code\":"), '\"');
    code_ValueEnd = strchr(code_ValueStart + 1, ',');
    sscanf(code_ValueStart + 1, "%d", &response_json.code);
}

// 数据包含{} 
char * agora_Start = NULL;
char * agora_ValueStart = NULL;
char * agora_ValueEnd = NULL;
agora_Start = strstr(data, "\"agoraRtcTokenDTO\":");
if(agora_Start != NULL){
    agora_ValueStart = strchr(agora_Start + strlen("\"agoraRtcTokenDTO\":"), '{');
    agora_ValueEnd = strchr(agora_ValueStart + 1, '}');
    *agora_ValueEnd = '\0';