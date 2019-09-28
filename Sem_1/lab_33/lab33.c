#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define NO_FLAGS 0
#define TIME_A 2
#define TIME_B 3
#define TIME_C 4
#define CHILD 0
#define PART_A 0
#define PART_B 1
#define PART_C 2
#define PART_MODULE 3
#define WAIT -1

#define ACCESS_RIGHTS 0600

pid_t partA;
pid_t partB;
pid_t partC;
pid_t modules;

sig_atomic_t volatile flag = 1;

void sig_handler(int signal)
{
    if(signal == SIGINT){
        printf("\nKill them all\n");
	    flag = 0;
        kill(partA, signal);
        kill(partB, signal);
        kill(partC, signal);
        kill(modules, signal);
    }
}


int delSem(int sem)
{
    if(semctl(sem, 0, IPC_RMID, 0) == -1){
        perror("Can't del semaphore");
        return 1;
    }

    return 0;
}

int startPart(char *executable, char *partName)
{
    pid_t pid = fork();

    if(pid == -1){
        perror("Can't start part");
        return -1;
    }

    if(pid != CHILD) {
        return pid;
    }

    execl(executable, executable, partName, NULL);
    perror("Can't execute part");
    return -1;
}

int factoryLoop(int sem)
{
    struct sembuf makeWidget[2] = {
            {PART_C, WAIT, SEM_UNDO},
            {PART_MODULE, WAIT, SEM_UNDO}
    };

    int i = 0;

    while(flag){
	    signal(SIGCONT, sig_handler);

        if(semop(sem, makeWidget, 2) != -1){
		    printf("Widget %d is done\n", i+1);
		    fflush(stdout);
	       	i++;
	    }
    }

    return 0;
}

int main()
{
    int sem = semget(getuid(), 4, IPC_CREAT | ACCESS_RIGHTS);

    if(sem == -1){
        perror("Can't get semaphore");
        return 1;
    }

    if((partA = startPart("lab33part", "a")) == -1){
        delSem(sem);
        return 1;
    }

    if((partB = startPart("lab33part", "b")) == -1){
        delSem(sem);
        return 1;
    }

    if((partC = startPart("lab33part", "c")) == -1){
        delSem(sem);
        return 1;
    }

    if((modules = startPart("lab33module", "")) == -1){
        delSem(sem);
        return 1;
    }

    signal(SIGINT, sig_handler);

    if(factoryLoop(sem) != 0){
    	delSem(sem);
	    return 1;
    }

    if(delSem(sem) != 0){
        return 1;
    }

    return 0;
}
