#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
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
#define PROJECT_PREFIX 123
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

int regInQueue(int queue)
{
    pid_t pid = getpid();
    struct message buf = {MSG_TYPE_REG, pid, ""};

    if(msgsnd(queue, &buf, sizeof(pid_t), NO_FLAGS) == -1)
    {
        perror("Can't register in queue");
        return 1;
    }

    return 0;
}

int getMessages(int queue, char *argv0)
{
    struct message msg;
    ssize_t code;
    pid_t pid = getpid();

    while((code = msgrcv(queue, &msg, SIZE_MSG + sizeof(pid_t), pid, NO_FLAGS)) >= 0)
    {
	//if message is empty
        if(code == sizeof(pid_t))
	    {
            break;
        }

        printf("%s, msg: %s\n", argv0, msg.mtext);
    }

    if(code == -1)
    {
        perror("Can't get message");
        return 1;
    }

    return 0;
}

int leaveQueue(int queue)
{
    pid_t pid = getpid();
    struct message buf = {MSG_TYPE_END_ANSWER, pid, ""};

    if(msgsnd(queue, &buf, sizeof(pid_t), NO_FLAGS) == -1){
        perror("Can't send message");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 1){
        fprintf(stderr, "Bad args\n");
        return 1;
    }

    key_t queueKey = ftok("send.c", PROJECT_PREFIX);

    if(queueKey == -1){
        perror("Can't generate key for queue");
        return 1;
    }

    int queue;
    queue = msgget(queueKey, NO_FLAGS);

    if(queue == -1){
        perror("Can't get queue");
        return 1;
    }

    if(regInQueue(queue) != 0){
        return 1;
    }

    if(getMessages(queue, argv[0]) != 0){
        return 1;
    }

    if(leaveQueue(queue) != 0){
        return 1;
    }

    return 0;
}
