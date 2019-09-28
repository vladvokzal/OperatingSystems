#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
		pid_t pid = fork();
		
		if (pid == -1){
				perror("fork error");
				return -1;
		}

		//"имя того, что мы исполняем", "arg0", "arg1", ... NULL
		if (pid == 0  && execlp("cat", "cat_for_user", "../lab_10/main.c", NULL) == -1){
				perror("execlp error");
				return -1;
		}

		printf(" This string was typed in parent procces \n");
		return 0;
}

