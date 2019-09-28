#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <limits.h> 
#include "31.h"

int main() {
	int queue = msgget(geteuid(), IPC_CREAT | 0666);

	if (MSGGET_ERROR == queue){
		perror("msgget error");
		return -1;
	}

	int workingSenders = 0;
	struct message buf;
	
	while (strcmp(buf.mtext, "Start work.")){
		if (MSGRCV_ERROR == msgrcv(queue, &buf, SIZE_MESS, TAKE_ALL, NO_FLAGS)){
			perror("msgrecv error");
			msgctl(queue, IPC_RMID, NULL);
			return -1;
		}
		++workingSenders;
		printf("Message \"%s\". Type %ld.\n", buf.mtext, buf.mtype);
	}

	while (workingSenders != NO_WORKING_SENDERS){
		if (MSGRCV_ERROR == msgrcv(queue, &buf, SIZE_MESS, TAKE_ALL, NO_FLAGS)){
			perror("msgrecv error");
			msgctl(queue, IPC_RMID, NULL);
			return -1;
		}
		if (!strcmp(buf.mtext, "End work.")){
			--workingSenders;
		}
		if (!strcmp(buf.mtext, "Start work.")){
			++workingSenders;
		}
		printf("Message \"%s\". Type %ld.\n", buf.mtext, buf.mtype);
	}

	if (MSGCTL_ERROR == msgctl(queue, IPC_RMID, NULL)){
		perror("msgctl error");
		return -1;
	}

	return 0;
}
