#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include "smap.h"

#define COMMAND(c) {c, #c}

struct command {
	unsigned int code;
	char *name;
};

struct command cmd_read[] = {
	COMMAND(SMAP_IOC_READ_PUT_USER),
	COMMAND(SMAP_IOC_READ_DIRECT),
	COMMAND(SMAP_IOC_READ_DISABLE_SMAP),
};

struct command cmd_write[] = {
	COMMAND(SMAP_IOC_WRITE_GET_USER),
	COMMAND(SMAP_IOC_WRITE_DIRECT),
	COMMAND(SMAP_IOC_WRITE_DISABLE_SMAP),
};

void help(char *argv0)
{
	int i;

	fprintf(stderr, "Usage: %s <device> <cmd> [<value>]\n\n", argv0);

	for (i = 0; i < sizeof(cmd_read) / sizeof(struct command); i++) {
		fprintf(stderr, "\t%s\n\t%s <value>\n",
			cmd_read[i].name, cmd_write[i].name);
	}
}

struct command * search_cmd(char *name, struct command cmd[], int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (strncmp(name, cmd[i].name, strlen(cmd[i].name)) == 0)
			return &cmd[i];
	}

	return NULL;
}

int ioctl_read(int fd, struct command *cmd)
{
	int value;

	if (ioctl(fd, cmd->code, &value) < 0) {
		perror(cmd->name);
		return errno;
	}

	printf("value = %d\n", value);

	return 0;
}

int ioctl_write(int fd, struct command *cmd, int value)
{
	if (ioctl(fd, cmd->code, &value) < 0) {
		perror(cmd->name);
		return errno;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int fd, value, ret = 0;
	char *device, *cmd_name;
	struct command *cmd;

	if (argc < 3) {
		help(argv[0]);
		return -1;
	}
	device = argv[1];
	cmd_name = argv[2];

	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror(device);
		return fd;
	}

	if (argc == 4) {
		value = atoi(argv[3]);

		cmd = search_cmd(cmd_name, cmd_write,
				sizeof(cmd_write) / sizeof(struct command));
		if (cmd) {
			ret = ioctl_write(fd, cmd, value);
		} else {
			fprintf(stderr, "Unknown write command: %s\n", cmd_name);
			ret = -1;
		}
	} else {
		cmd = search_cmd(cmd_name, cmd_read,
				sizeof(cmd_read) / sizeof(struct command));
		if (cmd) {
			ret = ioctl_read(fd, cmd);
		} else {
			fprintf(stderr, "Unknown read command: %s\n", cmd_name);
			ret = -1;
		}
	}

	close(fd);

	return ret;
}
