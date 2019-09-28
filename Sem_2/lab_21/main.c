#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define PHILO 5
#define FOOD 50
#define BUF_SIZE 256

#define CHECK_ERROR(status, mes)\
if (status != 0) {\
	char buf[BUF_SIZE] = { 0 };\
	strerror_r(status, buf, sizeof(buf));\
	fprintf(stderr, "%s: %s\n", mes, buf);\
	pthread_exit(NULL);\
}

pthread_mutex_t forks[PHILO];
pthread_mutex_t getting_forks_mx;
pthread_cond_t getting_forks_cond;

pthread_t phils[PHILO];

pthread_mutex_t foodlock;

int food_on_table() {
	static int food = FOOD;
	int myfood;
	int status;

	status = pthread_mutex_lock(&foodlock);
	CHECK_ERROR(status, "pthread_mutex_lock");

	if (food > 0) {
		food--;
	}
	myfood = food;
	
	status = pthread_mutex_unlock(&foodlock);
	CHECK_ERROR(status, "pthread_mutex_unlock");
	
	return myfood;
}

void get_forks(int phil, int fork1, int fork2) {
	int res;
	int status;

	status = pthread_mutex_lock(&getting_forks_mx);
	CHECK_ERROR(status, "pthread_mutex_lock");

	do {
		if (!(res = pthread_mutex_trylock(&forks[fork1]))) {
			res = pthread_mutex_trylock(&forks[fork2]);
			if (res) {
				status = pthread_mutex_unlock(&forks[fork1]);
				CHECK_ERROR(status, "pthread_mutex_unlock");
			}
		}
		if (res) {
			pthread_cond_wait(&getting_forks_cond, &getting_forks_mx);
		}
	} while(res);
	status = pthread_mutex_unlock(&getting_forks_mx);
	CHECK_ERROR(status, "pthread_mutex_unlock");
}

void down_forks(int f1, int f2) {
	int status;
	status = pthread_mutex_lock(&getting_forks_mx);
	CHECK_ERROR(status, "pthread_mutex_lock");
	status = pthread_mutex_unlock (&forks[f1]);
	CHECK_ERROR(status, "pthread_mutex_unlock");
	status = pthread_mutex_unlock (&forks[f2]);
	CHECK_ERROR(status, "pthread_mutex_unlock");
	pthread_cond_broadcast(&getting_forks_cond);
	status = pthread_mutex_unlock(&getting_forks_mx);
	CHECK_ERROR(status, "pthread_mutex_unlock");
}

void *philosopher(void *num) {

	int id;
	int left_fork, right_fork, food =0;
	int counter = 0;

	id = (int)num;
	printf("Philosopher %d sitting down to dinner.\n", id);
	right_fork = id;
	left_fork = id + 1;

	if (left_fork == PHILO) {
		left_fork = 0;
	}

	while (food = food_on_table () ) {
		if(counter >= (FOOD / PHILO)){
			sched_yield();
			break;
		}
		printf("Philosopher %d: get dish %d.\n", id, food);
		get_forks (id, right_fork, left_fork);

		printf("Philosopher %d: eating.\n", id);
		usleep ((FOOD - food + 1));
		down_forks (left_fork, right_fork);
		counter++;
	}

	printf("Philosopher %d is done eating.\n", id);
	printf("counter = %d\n", counter);

}

int main(int argn, char **argv) {
	int i;
	int status;

	status = pthread_mutex_init(&foodlock, NULL);
	CHECK_ERROR(status, "pthread_mutex_init");
	
	for (i = 0; i < PHILO; i++) {
		status = pthread_mutex_init(&forks[i], NULL);
		CHECK_ERROR(status, "pthread_mutex_init");
	}
	
	status = pthread_mutex_init(&getting_forks_mx, NULL);
	CHECK_ERROR(status, "pthread_mutex_init");
	pthread_cond_init(&getting_forks_cond, NULL);

	for (i = 0; i < PHILO; i++) {
		status = pthread_create(&phils[i], NULL, philosopher, (void *)i);
		CHECK_ERROR(status, "pthread_create");
	}
	
	for (i = 0; i < PHILO; i++) {
		status = pthread_join(phils[i], NULL);
		CHECK_ERROR(status, "pthread_join");
	}

	return 0;
}

/*

Как устроен trylock и как можно оптимизировать, чтобы все философы ели более менее равномерно

*/
