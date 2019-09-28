#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define num_steps 200000000

typedef union ThreadInfo {
		size_t id;
		double value;
} ThreadInfo;


int th_count;

void* calculate(void *param) {
	double local_pi = 0.0;
	size_t i = ((ThreadInfo *)param)->id;
	
	for (i; i < num_steps; i += th_count) {
			local_pi += 1.0 / (i * 4.0 + 1.0);
			local_pi -= 1.0 / (i * 4.0 + 3.0);
	}
	((ThreadInfo*)param)->value = local_pi;
	
	return param;
}

int main(int argc, char** argv) {
	
	double pi = 0;
	int i;
	pthread_t* ids;
	ThreadInfo* threads;
	
	if (argc != 2){
		printf("Usage: [prog_name] [number_of_threads] \n");
		pthread_exit(NULL);
	}

	if ((th_count = atoi(argv[1])) <= 0){
		printf("Second argument should be non-negative \n");
		pthread_exit(NULL);
	}
	
	threads = malloc(th_count * sizeof(ThreadInfo));
	ids = malloc(th_count * sizeof(pthread_t));				
	
	for( i = 0; i < th_count; i++) {
		threads[i].id = i;
		pthread_create(ids + i, NULL, calculate, (void*)(threads + i));
	}
	
	for(i = 0; i < th_count; i++) {
		ThreadInfo* res;
		thread_join(ids[i], (void**)&res);
		pi += res->value;
	}

	free(threads);
	free(ids);
	pi *= 4.0;
	printf ("pi = %.16f\n", pi);

	return 0;
}

