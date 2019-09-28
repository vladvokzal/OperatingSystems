#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

sem_t sem1;
sem_t sem2;

void* thread_body(void* param){
	int i;
	for(i = 0; i < 10; i++){
		//декремент
		if(sem_wait(&sem2) != 0){
			printf("Error\n");
			pthread_exit(NULL);
		}
		printf("Child: %i\n", i);
		//инкремент
		if(sem_post(&sem1) != 0){
			printf("Error\n");
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_t thread;
	int code;
	int i;
	
	if( (code = pthread_create(&thread, NULL, thread_body, NULL)) != 0){
		printf("Error in creating thread \n");
		pthread_exit(NULL);
	}

	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 0);

	for(i = 0; i < 10; i++){
		sem_wait(&sem1);
		printf("Parent: %i\n", i);
		sem_post(&sem2);
	}
	
	sem_destroy(&sem1);
	sem_destroy(&sem2);
	
	pthread_exit(NULL);
}
