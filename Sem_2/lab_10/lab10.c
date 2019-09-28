#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 256

#define CHECK_ERROR(status, mes)\
if (status != 0) {\
	char buf[BUF_SIZE] = { 0 };\
	strerror_r(status, buf, sizeof(buf));\
	fprintf(stderr, "%s: %s\n", mes, buf);\
	pthread_exit(NULL);\
}

char* child = "child";
char* parent = "parent";
pthread_mutex_t mutexes[3];
int printed = 0;

#define ITERATIONS 10

void* print_message(void* str) {
    int k = 1, status;

    status = pthread_mutex_lock(&mutexes[2]);
    CHECK_ERROR(status, "pthread_mutex_lock");

    if (printed == 1) {
        status = pthread_mutex_unlock(&mutexes[0]);
        CHECK_ERROR(status, "pthread_mutex_unlock");
    }

    for (int i = 0; i < ITERATIONS * 3; ++i) {
        status = pthread_mutex_lock(&mutexes[k]);
        CHECK_ERROR(status, "pthread_mutex_lock");

        k = (k + 1) % 3;

        status = pthread_mutex_unlock(&mutexes[k]);
        CHECK_ERROR(status, "pthread_mutex_unlock");

        if (k == 2) {
            printf("Message : %s\n", (char*)str);
            printed = 1;
        }

        k = (k + 1) % 3;
    }

    status = pthread_mutex_unlock(&mutexes[2]);
    CHECK_ERROR(status, "pthread_mutex_unlock");
}

void* child_fun(void* str) {
    int status = pthread_mutex_lock(&mutexes[0]);
    CHECK_ERROR(status, "pthread_mutex_lock");

    while (printed == 0) {
        sched_yield();
    }

    print_message(str);
}

int main (int argc, char* argv[]) {
    pthread_t thread;
    pthread_mutexattr_t mutexAttr;
    int status;

    status = pthread_mutexattr_init(&mutexAttr);
    CHECK_ERROR(status, "pthread_mutexattr_init");

    status = pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);
    CHECK_ERROR(status, "pthread_mutexattr_settype");

    for (int i = 0; i < 3; ++i) {
        status = pthread_mutex_init(&mutexes[i], &mutexAttr);
        CHECK_ERROR(status, "pthread_mutex_init");
    }

    status = pthread_create(&thread, NULL, child_fun, child);
    CHECK_ERROR(status, "pthread_create");

    print_message(parent);

    status = pthread_join(thread, NULL);
    CHECK_ERROR(status, "pthread_join");

    for (int i = 0; i < 3; ++i) {
        status = pthread_mutex_destroy(&mutexes[i]);
        CHECK_ERROR(status, "pthread_mutex_destroy");
    }

    pthread_exit(NULL);
}
