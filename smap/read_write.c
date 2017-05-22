#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "smap.h"

int main(int argc, char *argv[])
{
	int fd, value, ret = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <device> [<value>]\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror(argv[1]);
		return fd;
	}

	if (argc == 3) {
		value = atoi(argv[2]);
		if (ioctl(fd, SMAP_IOC_WRITE, &value) < 0) {
			perror("SMAP_IOC_WRITE");
			ret = errno;
		}
	} else {
		if (ioctl(fd, SMAP_IOC_READ, &value) < 0) {
			perror("SMAP_IOC_READ");
			ret = errno;
		} else {
			printf("value = %d\n", value);
		}
	}

	close(fd);

	return ret;
}