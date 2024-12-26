#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int fd;
	char buf[128];
	
	fd = open("fifo1", O_WRONLY);
	while(1){
		fgets(buf, 128, stdin);
		buf[strlen(buf) - 1] = '\0';
		write(fd, buf, sizeof(buf));
	}
	close(fd);
	return 0;
}
