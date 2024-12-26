#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>

int main()
{
	int fd1, fd2, fd3, max_fd = 0;
	int ret;
	char buf1[128], buf2[128], buf3[128];
	fd_set readfds, readfds_tmp;
	
	fd1 = open("fifo1", O_RDONLY);
	fd2 = open("fifo2", O_RDONLY);
	fd3 = open("fifo3", O_RDONLY);
	
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_tmp);
	
	FD_SET(fd1, &readfds);
	max_fd = (max_fd > fd1 ? max_fd : fd1);
	FD_SET(fd2, &readfds);
	max_fd = (max_fd > fd2 ? max_fd : fd2);
	FD_SET(fd3, &readfds);
	max_fd = (max_fd > fd3 ? max_fd : fd3);
	
	while(1){
		readfds_tmp = readfds;
		ret = select(max_fd + 1, &readfds_tmp, NULL, NULL, NULL);
		if(ret < 0){
			perror("select error");
			exit(-1);
		}
		
		if(FD_ISSET(fd1, &readfds_tmp)){
			memset(buf1, 0, sizeof(buf1));
			read(fd1, buf1, sizeof(buf1));
			printf("buf1 = %s\n", buf1);
		}
		if(FD_ISSET(fd2, &readfds_tmp)){
			memset(buf2, 0, sizeof(buf2));
			read(fd2, buf2, sizeof(buf2));
			printf("buf2 = %s\n", buf2);
		}
		if(FD_ISSET(fd3, &readfds_tmp)){
			memset(buf3, 0, sizeof(buf3));
			read(fd3, buf3, sizeof(buf3));
			printf("buf3 = %s\n", buf3);
		}
	}
	close(fd1);
	close(fd2);
	close(fd3);
	return 0;
}
