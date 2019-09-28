#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h> 

#define SIZE 128
#define ERR_FORK -1
#define ERR_PIPE -1 
#define ERR_CLOSE -1

int main(){

	int fdpipe[2];
	pid_t pid;
	
	char buffer[SIZE] = "sssQQQhelloworldeeeXZAQd";
	char chars[SIZE];

	printf("Was before toupper: %s\n", buffer);

	if(-1 == pipe(fdpipe)){
		perror("pipe");
		return ERR_PIPE;
	}
	
	pid = fork();
	
	if(-1 == pid){
		perror("fork");
		return ERR_FORK;
	}

	if(0 == pid){
		write(fdpipe[1], buffer, SIZE);
		
		if(-1 == close(fdpipe[1])){
			perror("close");
			return ERR_CLOSE;
		}
		return 0;
	}
	
	read(fdpipe[0], chars, SIZE);
	
	for(int i = 0; i < strlen(chars); i++){
		chars[i] = toupper(chars[i]);
	}

	printf("After toupper: %s\n", chars);

	if(-1 == close(fdpipe[0])){
		perror("close");
		return ERR_CLOSE;
	}

	return 0;
}
