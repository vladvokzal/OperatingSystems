#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include "30.h"

int main(){
	int queue = msgget(geteuid(), NO_FLAGS);
	if (MSGGET_ERROR == queue){
		perror("msgget error");
		return -1;
	}

	struct message buf;
	buf.mtype = getuid();
	while (strcmp(buf.mtext, "quit") != IS_SIMILAR) {
		scanf("%s", buf.mtext);
		if (MSGSND_ERROR == msgsnd(queue, &buf, strlen(buf.mtext) + 1, 0))
		{
			perror("msgsnd error");
			return -1;
		}
	}
    
	return 0;
}
