#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define MAX_FDC 256
#define BUFSZ 256
#define TIME_OUT 1

int main(int argc, char** argv) 
{
	FILE* openFiles[MAX_FDC];
	fd_set readfds;
	struct timeval timeout;
	int bytesRead;
	int curDescriptor;
	int stdoutDesc = 1;
	int fdc = 0;
	char buf[BUFSZ];
	char buf_read[BUFSZ];

	for (int i = 1; i < argc; i++) {
		openFiles[fdc] = fopen(argv[i], "rb");
		if (openFiles[fdc] == NULL){
			perror(argv[i]);
			return -1;
		} 
		else if (++fdc == MAX_FDC) {
			break;	
		}
	}

	//while(fdc){
	for (int i = 0; i < fdc; i++) 
	{
		timeout.tv_sec = TIME_OUT;
		timeout.tv_usec = 0;
			
		//get file decriptor from FILE*
		//curDescriptor = fileno(openFiles[i]);

		//clear a set
		//FD_ZERO(&readfds);
		//add file descriptor to a set
		//FD_SET(curDescriptor, &readfds);
			
		// read(curDescriptor, buf_read, sizeof(int));
			
		fgets(buf_read, BUFSZ, openFiles[i]);
		bytesRead = strlen(buf_read);
		write(stdoutDesc, buf_read, bytesRead);

		//monitor for multiple file descriptors
		// if (select(curDescriptor + 1, &readfds, NULL, NULL, &timeout) > 0) {
		// 	if (fgets(buf, BUFSZ, openFiles[i]) != 0) {
		// 		bytesRead = strlen(buf);
		// 		write(stdoutDesc, buf, bytesRead);
		// 	}
		// 	else {
		// 		close(curDescriptor);
		// 		openFiles[i--] = openFiles[--fdc];
		// 	}
		// }
	}
	//}
	return 0;
}