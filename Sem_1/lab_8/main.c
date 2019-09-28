#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define ERROR_OPEN_FILE -1
#define OPERATION_ERROR -1

int main(int argc, char *argv[]){
	struct flock lock;
	int fd, operationOnFd;
//	system("chmod +r text.txt");

	fd = open(argv[1], O_RDWR);

	if (fd == ERROR_OPEN_FILE) {
		perror(argv[1]);
		printf("Couldn't open file\n");
		return OPERATION_ERROR;
	}

	printf("File was opened\n");

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	operationOnFd = fcntl(fd, F_SETLK, &lock);
	
	if (operationOnFd == -1) {
		perror("fcntl");
		return OPERATION_ERROR;
	}

	system("nano text.txt");
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return EXIT_SUCCESS;
 }

