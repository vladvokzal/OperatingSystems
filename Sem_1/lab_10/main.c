#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define PARAM_NUMBER 2

int main(int argc, char* const argv[])
{

	if (argc < PARAM_NUMBER){
		printf("Write two arguments - [CommandName] [Args...] \n");
		return -1;
	}

	pid_t proccesID;
	int chldStatus = 0;

	proccesID = fork();

	//Fork returns an error
	if (proccesID == -1){
		perror("fork ERROR");
		return -1;
	}
	
	if (proccesID == 0 && execvp(argv[1], &argv[1]) == -1){
		perror("execvp ERROR");
		return -1;
	}

	//Если не сделать, то будет процесс-зомби до перезагрузки системы
	if(wait(&chldStatus) == -1){
		perror("Wait error");
		return -1;
	}

	return 0;
}
