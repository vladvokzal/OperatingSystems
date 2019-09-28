#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NUM_ITERATIONS 50000

pthread_t* th_array;
int* rank;
double* th_sums;
int th_count;

void* thread_body(void* param){
	double current_pi = 0.0;
	int th_id = *((int*)param);
	th_sums[th_id] = 0.0;
	int i;

	for(i = th_id; i < NUM_ITERATIONS; i += th_count){
		current_pi += 1.0 / (i * 4.0 + 1.0);
		current_pi -= 1.0 / (i * 4.0 + 3.0);
	}
	printf("current_pi = %f\n", current_pi);
	th_sums[th_id] += current_pi;

	pthread_exit(NULL);
}

double gather_summs(){
	int i;
	double result = 0.0;

	for (i = 0; i < th_count; ++i){
		result += th_sums[i];
	}

	return result;
}


int main(int argc, char const *argv[])
{
	double pi = 0.0;
	double final_pi;
	int code = 0;
	int i, j;

	if (argc != 2){
		printf("Usage: [prog_name] [number_of_threads] \n");
		pthread_exit(NULL);
	}

	if ((th_count = atoi(argv[1])) <= 0){
		printf("Second argument should be non-negative \n");
		pthread_exit(NULL);
	}
	printf("th_count = %d\n", th_count);
	th_array = malloc(sizeof(pthread_t) * th_count);
	th_sums = malloc(sizeof(double) * th_count);
	rank = (int *)malloc(sizeof(int) * th_count);

	if (th_array == NULL || th_sums == NULL){
		printf("Memory allocation failed \n");
		pthread_exit(NULL);
	}

	for (int th_id = 0; th_id < th_count; ++th_id){
		rank[th_id] = th_id;
		if( (code = pthread_create(th_array + th_id, NULL, thread_body, &(rank[th_id]))) != 0){
			printf("Error\n");
			exit(1);
		}

	}

	for (int i = 0; i < th_count; ++i){
		pthread_join(th_array[i], (void **)&pi);
		final_pi += pi;
	}

	final_pi *= 4.0;
	printf("Calculated PI value = %0.16f\n", final_pi);

	free(th_array);
	free(th_sums);
	free(rank);

	return 0;
}