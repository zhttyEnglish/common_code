/* 将字符串 转换为 16进制    例如: char a[]= "123456789" 转换为 unsigned char b[]={0x31, 0x32 .......}*/
int StringToHex(char *str, unsigned char *out, int *outlen)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    tmplen = strlen(p);
    while(cnt < (tmplen / 2))
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p ++;
        cnt ++;
    }
    if(tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
 
    if(outlen != NULL) *outlen = tmplen / 2 + tmplen % 2;
    return tmplen / 2 + tmplen % 2;
}
/*================================================================================================================================*/


/* 将字符串转换为两位16进制 例如: char a[]="123456" 转换为 char b[]={0x33,0x31,0x33,0x32,0x33,0x33,0x33,0x34,0x33,0x35......}*/
void str2hex(char * dest, char * src, int len)
{
	int i, j;
	for(i = 0, j = 0; i < len; i ++){
        sprintf(&dest[j], "%02x", src[i]);
        j+=2; // 每个16进制占2个长度
	}
	dest[j] = '\0';
}
/*================================================================================================================================*/


/* 将16进制数组转换为字符串 例如: unsigned char a[]={0x31,0x32,0x33,0x34} 转换为 char b[]="1234" */
int hex_to_ascii(unsigned char * hex, int len, char * ascii)
{
	 
	for(int i = 0; i < len; i ++){
		 if((hex[i] >= 0x30) && (hex[i] <= 0x39))
	        ascii[i] = '0' + hex[i] - 0x30;
	    else if((hex[i] >= 0x41) && (hex[i] <= 0x5A)) // capital
	        ascii[i] = 'A' + (hex[i] - 0x41);
	    else if((hex[i] >= 0x61) && (hex[i] <= 0x7A)) // little case
	        ascii[i] = 'a' + (hex[i] - 0x61);
	    else 
	        ascii[i] = 0xff;
	}
   
    return len;
}
/*================================================================================================================================*/

/* 16进制转为字符串 例如 uint8_t a[]={0xab, 0xcd, 0xef} 转换为 char b[]="abcdef" */
void hex2str(uint8_t *input, uint16_t input_len, char *output)
{
    char *hexEncode = "0123456789ABCDEF";    //修改abcdef  可以改变 大小写
    int i = 0, j = 0;

    for (i = 0; i < input_len; i++) {
        output[j++] = hexEncode[(input[i] >> 4) & 0xf];
        output[j++] = hexEncode[(input[i]) & 0xf];
    }
}
/*================================================================================================================================*/