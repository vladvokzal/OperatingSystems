#define _XOPEN_SOURCE 600
#include <signal.h> 
#include <stdio.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int n;

void catchSIGQUIT(int signal){
	++n;
	if (write(STDOUT_FILENO, "\07", sizeof(char)) == -1){
		perror("write error");
		raise(signal);
		_exit(1);
	}
	return;
	//kill(getppid(), signal);
}

void catchSIGINT(int signal){
	printf("\n Number of SIGQUIT: %d\n", n);
	raise(signal);
	//kill(getpid(), signal);
	//kill(getppid(), signal);
	_exit(1);
}

int main() {
	
	if (sigset(SIGQUIT, catchSIGQUIT) == SIG_ERR){
		return -1;
	}
	
	if (sigset(SIGINT, catchSIGINT) == SIG_ERR){
		return -1;
	}

	while(1);

	return 0;
}
