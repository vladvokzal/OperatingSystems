#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define NUM_STEPS 1000000


typedef union ThreadInfo {
	size_t id;
	double value;
} ThreadInfo;


int th_count;
int quit_flag = 0;
long long maxiterations = 0;
pthread_mutex_t maxiter_mutex = PTHREAD_MUTEX_INITIALIZER;


void* calculate(void *param) {
	
	double localpi = 0.0;
	long long threadno = ((ThreadInfo *)param)->id;
	long long i = threadno;
	long long tocheck;
	pthread_mutex_lock(&maxiter_mutex);

	while(!quit_flag || i < maxiterations + threadno) {
		tocheck= i + NUM_STEPS * th_count;

		if (tocheck > maxiterations && !quit_flag){
			maxiterations = tocheck;
		}

		pthread_mutex_unlock(&maxiter_mutex);
		
		for (i; i < tocheck; i += th_count) {
			localpi += 1.0 / (i * 4.0 + 1.0);
			localpi -= 1.0 / (i * 4.0 + 3.0);
		}

		pthread_mutex_lock(&maxiter_mutex);
	}
	
	pthread_mutex_unlock(&maxiter_mutex);
	
	((ThreadInfo *)param)->value = localpi;
	return param;
}

void handlesigint2(int sig) {
	fputs("Wait, I'm quitting right now...\n", stderr);
	signal(sig, handlesigint2);
}

void signal_handler(int sig) {
	quit_flag = 1;
	signal(sig, handlesigint2);
}


int main(int argc, char** argv) {
	
	double pi = 0;
	int i;
	int code = 0;
	pthread_t* ids;
	ThreadInfo* params = NULL;

	if (argc != 2){
		printf("Usage: [prog_name] [thread_number] \n");
		pthread_exit(NULL);
	}

	if( (th_count = atol(argv[1])) <= 0 ){
		printf("Write only nuber at 2-nd argument! \n");
		pthread_exit(NULL);
	}

	// if(th_count > 30){
	// 	printf("Too many threads - write less than 30 \n");
	// 	pthread_exit(NULL);
	// }

	signal(SIGINT, signal_handler);
	
	params = malloc(th_count * sizeof(ThreadInfo));
	ids = malloc(th_count * sizeof(pthread_t));

	if (!params || !ids){
		printf("Error in allocating memory...\n");
		pthread_exit(NULL);
	}

	for(i = 0; i < th_count; i++) {
		params[i].id = i;
		if( (code = pthread_create(ids + i, NULL, calculate, (void*)(params + i))) != 0){
			fprintf(stderr, "%s: Error in thread creation\n", argv[0]);
			pthread_exit(NULL);
		}
	}
	
	for(i = 0; i < th_count; i++) {
		ThreadInfo* res;
		pthread_join(ids[i], (void **)&res);
		pi += res->value;
	}

	pi *= 4.0;
	printf ("\nCalculated pi = %.15f\n", pi);

	pthread_mutex_destroy(&maxiter_mutex);
	pthread_exit(NULL);

}