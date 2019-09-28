#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
	
#define BUF_SIZE 256
#define LINE_COUNT 10

void* thread_body(void* param)
{
	int i;
	for(i = 0; i < LINE_COUNT; i++){
		printf("Child: %d\n", i);	
	} 
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread;
	int code;
	char buf[BUF_SIZE];
	int i;
	   
	code = pthread_create(&thread, NULL, thread_body, NULL);
	if (code != 0){
		strerror_r(code, buf, sizeof(buf));
		fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
		exit(1);
	}
				
	for(i = 0; i < LINE_COUNT; i++){
		printf("Parent: %i\n", i);	
	}

	pthread_exit(NULL);   
	return EXIT_SUCCESS;
}
