#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

extern char** environ;

int execvpe(char* file, char** argv, char** envp) {
	char** oldEnviron = environ;
	environ = envp;

	if (execvp(file, argv) == -1){
		environ = oldEnviron;
		perror("execvp error");
		return -1;
	}
	return 0;
}

int main(int argc, char** argv) {
	
	if (argc < 2){
		printf("Usage: ./prog [CommandName] [Args...] \n");
		return -1;
	}
	
	char* newEnviron[] = {"TZ=America/Los_Angeles", NULL};

	if (execvpe(argv[1], &argv[1], newEnviron) == -1){		
		return -1;
	}

	return 0;
}


