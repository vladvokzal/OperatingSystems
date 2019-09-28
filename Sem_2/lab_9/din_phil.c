#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PHILO 6
#define LEFT(i) ((i - 1) % PHILO) 
#define RIGHT(i) ((i + 1) % PHILO) 
#define DELAY 30000
#define FOOD 50
#define THINKING 1
#define HUNGRY 2
#define EATING 3


int food = FOOD;
pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
int state[PHILO];
void *philosopher(void *id);
int food_on_table();
void get_fork(int);
void down_forks(int);
void test(int);
pthread_mutex_t foodlock;

int sleep_seconds;

int main (int argc, char **argv)
{
	int i;
	int code;

	if( argc != 2 && argc != 1){
		printf("Usage: prog_name [optional] sleep_seconds \n");
		pthread_exit(NULL);
	} 

	if (argc == 2){
		if( (sleep_seconds = atol(argv[1])) <= 0){
			printf("Write only number at 2-nd argument! \n");
			pthread_exit(NULL);
		}
  	}

	pthread_mutex_init (&foodlock, NULL);
	for (i = 1; i < PHILO; i++){
			pthread_mutex_init (&forks[i], NULL);
	}
	
	for (i = 1; i < PHILO; i++){
		if( (code = pthread_create (&phils[i], NULL, philosopher, (void *)&i)) != 0){
			printf("Error in creating thread\n");
			pthread_exit(NULL);
		}
	}
	for (i = 1; i < PHILO; i++){
		if( (code = pthread_join (phils[i], NULL)) != 0){
			printf("Error in joining thread\n");
			pthread_exit(NULL);
		}
	}

	pthread_mutex_destroy(&foodlock);	
	pthread_exit(NULL);
}

int food_on_table(){
	static int food = FOOD;
	int myfood;

	pthread_mutex_lock (&foodlock);

	if (food > 0){
		food--;
	}
	myfood = food;

	pthread_mutex_unlock (&foodlock);
	return myfood;
}

void* philosopher (void* num){
	
	int id;
	int counter = 0;
	int f = 0;
	id = *(int*)num;
	printf ("Philosopher sitting down to dinner.\n");

	while ( f = food_on_table() ){
		sleep (sleep_seconds);
		
		printf ("Philosopher: get dish.\n");
		get_fork(id);

		printf ("Philosopher: eating.\n");
		usleep (DELAY);
		down_forks(id);
		counter++;
	}

	printf("counter = %d\n", counter);
	printf ("Philosopher is done eating.\n");

	return (NULL);
}

void get_fork(int phil){
	
	pthread_mutex_lock(&foodlock);
	state[phil] = HUNGRY;
	test(phil);
	pthread_mutex_unlock(&foodlock);
	
	pthread_mutex_lock (&forks[phil]);
	
	printf ("Philosopher: got fork %d and %d \n", LEFT(phil), RIGHT(phil));
}

void down_forks(int index){
	
	pthread_mutex_lock(&foodlock);
	state[index] = THINKING;
	test(LEFT(index));
	test(RIGHT(index));
	pthread_mutex_unlock(&foodlock);

	pthread_mutex_unlock (&forks[index]);
}

void test(int index){
	if(state[index] == HUNGRY && state[LEFT(index)] != EATING && state[RIGHT(index)] != EATING){
		state[index] = EATING;
		pthread_mutex_unlock(&forks[index]);
	}
}