#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	
	int pid = fork();
	if (pid == -1){
			perror("fork error");
			return -1;
	}

	if (pid == 0  && execlp("cat", "cat", "../lab_10/main.c", NULL) == -1){
			perror("execl error");
			return -1;
	}

	if (wait(NULL) == -1){
			perror("wait error");
			return -1;
	}

	printf(" This string was typed in parent procces ! \n");
	return 0;

}

