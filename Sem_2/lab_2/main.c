#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
	
#define BUF_SIZE 256
#define ITERATIONS 10

void* thread_body(void* param)
{
	int i;
	for(i = 0; i < ITERATIONS; i++){
		printf("Child: %d\n", i);	
	} 
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t chld_thread;
	int code;
	char buf[BUF_SIZE];
	int i;
	   
	code = pthread_create(&chld_thread, NULL, thread_body, NULL);
	if (code != 0){
		strerror_r(code, buf, sizeof(buf));
		fprintf(stderr, "%s: pthread_createating thread: %s\n", argv[0], buf);
		exit(1);
	}
	
	pthread_join(chld_thread, NULL);

	for(i = 0; i < ITERATIONS; i++){
		printf("Parent: %i\n", i);	
	}

	pthread_exit(NULL);   
	return EXIT_SUCCESS;
}