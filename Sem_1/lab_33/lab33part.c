#include <stdio.h>
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
    char partName = argv[1][0];

    int sem = semget(getuid(), 4, NO_FLAGS);

    if(sem == -1)
    {
        perror("Can't get semaphore");
        return 1;
    }

    int times[] = {TIME_A, TIME_B, TIME_C};

    int index = partName - 'a';

    struct sembuf part = {index, 1, SEM_UNDO};

    int i = 0;

    while(1)
    {
        sleep(times[index]);

        printf("Part %c number %d was made\n", partName, i + 1);
        i++;
        fflush(stdout);

	if (semop(sem, &part, 1) == -1)
	{
	     printf("Part %c end work\n", partName);
	     fflush(stdout);
	     return 0;
	}
    }
}

