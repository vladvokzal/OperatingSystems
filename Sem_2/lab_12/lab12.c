#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mt;
pthread_cond_t chst;
int isParent = 1;

void *print_msg(void *arg){
	int i = 0;
	
	if((char*)arg == "child" && isParent == 1){
		sched_yield();
	}

	pthread_mutex_lock(&mt);
	isParent = 0;
	for(i = 0; i < 10; i++){
		pthread_cond_signal(&chst);
		printf("%s %d\n", (char *)arg, i);
		pthread_cond_wait(&chst, &mt);
	}
	
	pthread_mutex_unlock(&mt);
	pthread_cond_signal(&chst);
	pthread_exit(NULL);
}

void mutex_destroy(){
	pthread_mutex_destroy(&mt);
	pthread_cond_destroy(&chst);
}

int main (int argc, char** argv)
{
	pthread_t thread_id;
	int ret;
	int i = 0;
	
	pthread_mutex_init(&mt, NULL);
	pthread_cond_init(&chst, NULL);
	
	if((ret = pthread_create(&thread_id, NULL, print_msg, (void*)"child")) != 0){
		printf("Error in creating thread \n");
		pthread_exit(NULL);
	}
	
	print_msg((void *)"parent");

	if( (ret = pthread_join(thread_id, NULL)) != 0){
		printf("Error in joining thread\n");
		pthread_exit(NULL);
	}

	mutex_destroy();
	
	pthread_exit(NULL);
}
