#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define log(format, ...) \
  			printf("[%s:%d] "format, __func__, __LINE__,  ##__VA_ARGS__)

#define array_log(data, len) \
			printf("[%s:%d:%s] ", __func__, __LINE__, "array_log");\
			for(int i = 0; i < len; i ++){printf("%d ", data[i]);} \
			printf("\r\n");
#else
#define	log(format, ...)
#define array_log(data, len)
#endif