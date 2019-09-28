#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char **argv)
{
    int sem = semget(getuid(), 4, NO_FLAGS);

    if(sem == -1)
    {
        perror("Can't get semaphore");
        return 1;
    }

    struct sembuf partAB[2] = {
            {PART_A, WAIT, SEM_UNDO},
            {PART_B, WAIT, SEM_UNDO}
    };

    struct sembuf module = {PART_MODULE, 1, SEM_UNDO};

    int i = 0;

    while(1)
    {
	if (semop(sem, &partAB[0], 2) != -1)
	{
            printf("Module %d was made\n", i + 1);
            fflush(stdout);
            i++;

             if (semop(sem, &module, 1) == -1)
	     {
	         printf("Module end work\n");
	     	 fflush(stdout);
	     	 return 0;
	     }
	}
    }
}
