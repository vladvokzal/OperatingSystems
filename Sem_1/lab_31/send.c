#include <sys/types.h>
#include <signal.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SIZE_MSG 100
#define EQUALS_STR 0
#define NO_FLAGS 0
#define MSG_TYPE_DATA 1
#define MSG_TYPE_REG 2
#define MSG_TYPE_END 3
#define MSG_TYPE_END_ANSWER 4
#define LENGTH_QUIT_MSG 5
#define QUEUE_RIGHTS 0600
#define PROJECT_PREFIX 123
#define ARRAY_RATIO_SIZE 2
#define TRUE 1
#define FALSE 0

struct message
{
	long mtype;
	pid_t pid;
	char mtext[SIZE_MSG];
};

struct array
{
	pid_t *arr;
	int indexForAdd;
	int size;
};

int closeQueue(int queue)
{
	if(msgctl(queue, IPC_RMID,NULL) == -1)
	{
		perror("Can't close queue");
		return 1;
	}

	return 0;
}

size_t msgLength(char *msg)
{
	size_t length = strlen(msg);

	if(msg[length - 1] == '\n')
	{
		msg[length - 1] = '\0';
	}
	else
	{
		++length;
	}

	return length;
}

struct array * initArray()
{
	struct array *res = calloc(1, sizeof(struct array));

	if(res == NULL)
	{
		perror("Can't init array");
		return NULL;
	}

	return res;
}

int pushBack(struct array *arr, pid_t item)
{
	if(arr->indexForAdd < arr->size)
	{
		arr->arr[arr->indexForAdd] = item;
		++arr->indexForAdd;
		return 0;
	}

	pid_t *tmp = (pid_t *)realloc(arr->arr, arr->size + ARRAY_RATIO_SIZE);

	if(tmp == NULL)
	{
		perror("Can't add new element");
		return 1;
	}

	arr->size += ARRAY_RATIO_SIZE;
	arr->arr = tmp;
	arr->arr[arr->indexForAdd] = item;
	++arr->indexForAdd;
	return 0;
}

int find(struct array *arr, pid_t item)
{
	int i;
	for(i = 0; i < arr->indexForAdd; ++i){
		if(arr->arr[i] == item){
			return i;
		}
	}

	return -1;
}

int removeItem(struct array *arr, int index)
{
	if(index < 0 || index >= arr->indexForAdd){
		return 1;
	}

	int i;
	for(i = index + 1; i < arr->indexForAdd; ++i){
		arr->arr[i - 1] = arr->arr[i];
	}

	--arr->indexForAdd;
	return 0;
}

void destroyArray(struct array *arr)
{
	free(arr->arr);
	free(arr);
}

int isEmpty(struct array *arr)
{
	return arr->indexForAdd <= 0;
}

int addListeners(int queue, struct array *pids)
{
	struct message buf;

	while(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_REG, IPC_NOWAIT) != -1){
		if(pushBack(pids, buf.pid) != 0){
			return 1;
		}
	}

	return 0;
}

int delListeners(int queue, struct array *pids)
{
	struct message buf;

	while(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_END, IPC_NOWAIT) != -1){
		int index = find(pids, buf.pid);

		if(index == -1){
			fprintf(stderr, "Del listener, which doesn't exits\n");
			return 1;
		}

		if(removeItem(pids, index) != 0){
			return 1;
		}
	}

	return 0;
}

int updListeners(int queue, struct array *pids)
{
	if(addListeners(queue, pids) != 0){
		return 1;
	}

	if(delListeners(queue, pids) != 0){
		return 1;
	}

	return 0;
}

int sendMessage(int queue, struct array *pids, struct message *buf, size_t length)
{	
	int i;
	for(i = 0; i < pids->indexForAdd; ++i){
		buf->mtype = pids->arr[i];
		if(msgsnd(queue, buf, length + sizeof(pid_t), NO_FLAGS) == -1){
			perror("Can't send message");
			return 1;
		}
	}

	printf("Message was sent to %d listener(s)\n", i);

	return 0;
}

int sendMessages(int queue, struct array *pids)
{
	printf("Enter messages (max length - %d symbols, Ctrl+D - for exit):\n", SIZE_MSG - 1);

	struct message buf;
	int isWork = TRUE;

	while(isWork){
		size_t length;

		if(fgets(buf.mtext, SIZE_MSG, stdin) == NULL){
			length = 0;
		}
		else{
			length = msgLength(buf.mtext);
		}

		if(updListeners(queue, pids) != 0){
			return 1;
		}

		if(length == 0){
			isWork = FALSE;
		}

		else if (buf.mtext[0] == '.'){
			buf.mtext[0] = '\0';
			isWork = FALSE;
		}

		if(sendMessage(queue, pids, &buf, length) != 0){
			return 1;
		}

	}

	return 0;
}

int waitAnswers(int queue, struct array *pids)
{
	struct message buf;

	while(!isEmpty(pids)){

		if(msgrcv(queue, &buf, sizeof(pid_t), MSG_TYPE_END_ANSWER, NO_FLAGS) == -1){
			perror("Can't get answer from listeners");
			return 1;
		}

		int index = find(pids, buf.pid);
		
		if(index == -1){
			fprintf(stderr, "Del listener, which doesn't exits\n");
			return 1;
		}

		if(removeItem(pids, index) != 0){
			return 1;
		}
	}

	return 0;
}

int main()
{
	key_t queueKey = ftok("send.c", PROJECT_PREFIX);

	if(queueKey == -1){
		perror("Can't generate key for queue");
		return 1;
	}

	int queue;
	queue = msgget(queueKey, (IPC_CREAT | QUEUE_RIGHTS));

	if(queue == -1){
		perror("Can't make queue");
		return 1;
	}

	printf("Queue was created\n");

	struct array *pids = initArray();

	if(sendMessages(queue, pids) != 0){
		closeQueue(queue);
		destroyArray(pids);
		return 1;
	}

	if(waitAnswers(queue, pids) != 0){
		closeQueue(queue);
		destroyArray(pids);
		return 1;
	}

	if(closeQueue(queue) != 0){
		destroyArray(pids);
		return 1;
	}

	destroyArray(pids);
	
	return 0;
}
