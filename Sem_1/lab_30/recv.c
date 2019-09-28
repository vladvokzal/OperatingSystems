#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include "30.h"

int main() {
	int queue = msgget(geteuid(), IPC_CREAT | 0666);
	if (MSGGET_ERROR == queue)
	{
		perror("msgget error");
		return -1;
	}

	struct message buf;
	while (strcmp(buf.mtext, "quit") != IS_SIMILAR){
		int res = msgrcv(queue, &buf, SIZE_MESS, TAKE_ALL, NO_FLAGS);
		if (MSGRCV_ERROR == res){
			perror("msgrecv error");
			msgctl(queue, IPC_RMID, NULL);
			return -1;
		}
		printf("Message \"%s\". Type %d. Length %d.\n", buf.mtext, buf.mtype, res);
	}

        if (MSGCTL_ERROR == msgctl(queue, IPC_RMID, NULL)) {
                perror("msgctl error");
                return -1;
        }


	return 0;
}
