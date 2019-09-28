#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define semname0 "/sem0"
#define semname1 "/sem1" 
#define CONSOLE 1

char* child_msg = "Child\n";
char* parent_msg = "Parent\n";

void unlink_all_sem(void) {
	sem_unlink(semname0);
	sem_unlink(semname1);
}

void close_all_sem(sem_t* sem1, sem_t* sem2){
	sem_close(sem1);
	sem_close(sem2);
}

int main() {
	sem_t* sem0;
	sem_t* sem1;  
	int i;
	
	sem0 = sem_open(semname0, O_CREAT | O_EXCL, 0600, 1);
	if (sem0 == SEM_FAILED){
		perror("ERROR in sem_open");
		exit(1);
	}

	sem1 = sem_open(semname1, O_CREAT | O_EXCL, 0600, 0);
	if (sem1 == SEM_FAILED){
		perror("ERROR in sem_open");
		exit(1);
	}
	
	pid_t child = fork();
	
	if (child < 0) {
		perror("ERROR in fork");
		exit(1);
	}

	if (child == 0) {
		for(i = 0; i < 10; i++) {
			sem_wait(sem1);
			write(CONSOLE, child_msg, strlen(child_msg));
			sem_post(sem0);
		}
	} else {
		for(i = 0; i < 10; i++) {
			sem_wait(sem0);
			write(CONSOLE, parent_msg, strlen(parent_msg));
			sem_post(sem1);
		}
		if(wait(NULL) == -1){
			perror("ERROR in wait");
			exit(1);
		}
	}


	close_all_sem(sem0, sem1);
	unlink_all_sem();
	
	return 0;
}

/*
	Посмотреть что такое sem_t  и почему мы из другого процесса можем доступиться к семафору, без еще одного открытия.
	Закртыть правильно семафоры.

*/

/*

Ответ что такое sem_t
typedef union
{
  char __size[__SIZEOF_SEM_T];
  long int __align;
} sem_t;

*/
