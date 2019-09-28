#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 80
#define SLEEP_TIME 5000
#define BUFFER_SIZE 256
#define STANDART_OUTPUT 1

#define CHECK_ERROR(status, mes)\
if (status != 0) {\
	char buf[BUF_SIZE] = { 0 };\
	strerror_r(status, buf, sizeof(buf));\
	fprintf(stderr, "%s: %s\n", mes, buf);\
	pthread_exit(NULL);\
}

typedef struct node{
	char *string;
	struct node *next;
	pthread_mutex_t mutex;
} Node;

typedef struct list{
	Node* head;
	int size;
	pthread_mutex_t mutex;
} List;


void swap(Node *a, Node *b){
	
	char *tmp = a->string;
	a->string = b->string;
	b->string = tmp;
}

void sort(List* list){
	Node* cur;
	int i;
	int status;
	int flag = 1;

	while(flag){
		flag = 0;
		cur = list->head;
		while(cur->next != NULL){
			status = pthread_mutex_lock(&(cur->mutex));
			CHECK_ERROR(status, "pthread_mutex_lock");
			status = pthread_mutex_lock(&(cur->next->mutex));
			CHECK_ERROR(status, "pthread_mutex_lock");
			
			if(strcmp(cur->string, cur->next->string) > 0){
				swap(cur, cur->next);
				flag = 1;
			}
			
			status = pthread_mutex_unlock(&(cur->next->mutex));
			CHECK_ERROR(status, "pthread_mutex_unlock");
			status = pthread_mutex_unlock(&(cur->mutex));
			CHECK_ERROR(status, "pthread_mutex_unlock");
			
			cur = cur->next;
		}
		
	}
}

void* func(void* param){
	int status;
	List* list = (List* )param;

	while(1){
		/* First way to avoid segmentation fault - see next comment why */
		if (list->size > 0){
			usleep(6000000);
			sort(list);
		}
		/* Second way */
		/* If we dont do this long usleep - get the segmentation fault in case of trying to acces non-allocated list->head->next */
		// usleep(6000000);
		// sort(list);
	}
}

void add(List* list, char* str){
	int status;
	char *tmpStr = (char *)malloc(strlen(str) + 1);
	
	if(tmpStr == NULL){
		fprintf(stderr, "ERROR in malloc\n");
		exit(1);
	}
	strcpy(tmpStr, str);
	Node* newHead = (Node *)malloc(sizeof(Node));

	if(newHead == NULL){
		fprintf(stderr, "ERROR in malloc\n");
		exit(1);
	}

	if(list->size > 0){
		status = pthread_mutex_lock(&(list->head->mutex));
		CHECK_ERROR(status, "pthread_mutex_lock");
	}

	status = pthread_mutex_init(&(newHead->mutex), NULL);
	CHECK_ERROR(status, "pthread_mutex_init");

	newHead->string = tmpStr;
	newHead->next = list->head;
	list->head = newHead;
	list->size++;

	if(list->size > 1){
		status = pthread_mutex_unlock(&(list->head->next->mutex));
		CHECK_ERROR(status, "pthread_mutex_unlock");	
	}
}

void output(List *list){
	char* msg = "Entered strings:\n";
	Node* cur = list->head;
	write(STANDART_OUTPUT, msg, strlen(msg));
	
	while(cur != NULL){
		write(STANDART_OUTPUT, cur->string, strlen(cur->string));
		cur = cur->next;
	}
}

List* create_list(){
	List* list = (List *)malloc(sizeof(List));

	if(list == NULL){
		fprintf(stderr, "ERROR in malloc\n");
		exit(1);
	}

	list->head = NULL;
	list->size = 0;

	return list;	
}

void free_list(List* list) {
	Node* tmp = list->head;
	Node* prev = NULL;
	
	if (NULL == tmp) {
		return;
	}
	
	while (tmp->next) {
		prev = tmp;

		tmp = tmp->next;
		
		free(prev->string);
		free(prev);
	}
	
	free(tmp->string);
	free(tmp);
}

int main(int argc, char** argv) {
	pthread_t thread;
	int code;
	char* msg = "Enter strings:\n";
	char cur_symbol = '\n';
	int i;
	int threads_number;

	if (argc != 2){
		printf("Usage: [prog_name] [threads_number] \n");
		return -1;
	}

	if ((threads_number = atoi(argv[1])) < 0){
		printf("Invalid threads number \n");
		return -1;
	}

	pthread_t* threads = (pthread_t *)malloc(threads_number * sizeof(pthread_t));
	
	List* list = create_list();
	
	code = pthread_mutex_init(&(list->mutex), NULL);
	CHECK_ERROR(code, "pthread_mutex_init");

	for (i = 0; i < threads_number; i++){
		code = pthread_create(&threads[i], NULL, func, (void *)list);
		CHECK_ERROR(code, "pthread_create");
	}

	write(STANDART_OUTPUT, msg, strlen(msg));
	char buf[BUF_SIZE] = { 0 };
	
	while(1){
		
		if(fgets(buf, BUF_SIZE, stdin) == NULL){
			fprintf(stderr, "fgets failed\n");
			return -1;
		}
		
		if((buf[0] == '\n') && (cur_symbol == '\n')){
			output(list);
			continue;
		}
		
		cur_symbol = buf[strlen(buf) - 1];
		add(list, buf);
	}

	free_list(list);
	return 0;
}

/*
Сделать несколько сортирующих потоков
*/