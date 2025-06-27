#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define log(format, ...) \
  			printf("[%s:%d] "format, __func__, __LINE__,  ##__VA_ARGS__)
#else
#define	log(format, ...)
#endif