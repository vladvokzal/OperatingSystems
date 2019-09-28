#ifndef IS_DEFINED
#define IS_DEFINED

#define NO_FLAGS 0
#define SIZE_MESS 100
#define MSGGET_ERROR -1
#define MSGSND_ERROR -1
#define MSGRCV_ERROR -1
#define IS_SIMILAR 0
#define TAKE_ALL 0
#define MSGCTL_ERROR -1

struct message {
        long mtype;
        char mtext[SIZE_MESS];
};

#endif
