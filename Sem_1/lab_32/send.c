#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h> 
#include <time.h> 
#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include "31.h"

int main(){
	int queue = msgget(getuid(), IPC_CREAT | 0666);
	
	if (MSGGET_ERROR == queue){
		perror("msgget error");
		return -1;
	}

	struct message buf;
	buf.mtype = getpid();
	sprintf(buf.mtext, "Start work.");
	if (MSGSND_ERROR == msgsnd(queue, &buf, strlen(buf.mtext) + 1, NO_FLAGS)){
		perror("msgsnd error");
		return -1;
	}

	srand(time(NULL));

	for (int i = 0; i < 10; ++i){
		sleep(1);
		sprintf(buf.mtext, "%d", rand());
		if (MSGSND_ERROR == msgsnd(queue, &buf, strlen(buf.mtext) + 1, NO_FLAGS)){
			perror("msgsnd error");
			return -1;
		}
	}

	sprintf(buf.mtext, "End work.");
		if (MSGSND_ERROR == msgsnd(queue, &buf, strlen(buf.mtext) + 1, NO_FLAGS)){
		perror("msgsnd error");
		return -1;
	}

	return 0;
}
