#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ZERO 0
#define ERR_SET_TZ -1
#define ERR_FUN_TIME -2

int main(int argc, char** argv)
{
	time_t currentTime;
	struct tm *pTime;	

	if(unsetenv("Mars/North") == 0)
	{
		printf("ERR \n");
		return -3;
	}

	/* Set time zone. If putenv returns nonzero value - error in setting time zone */
	if(putenv("TZ=PST8PDT") != ZERO)
	{
		printf("Error in setting time zonw, check it again \n");
		return ERR_SET_TZ;
	}

	/* Get time and checking for errors */
	if(time(&currentTime) == (time_t)-1 )
	{
		printf("Getting_Time error \n");
		return ERR_FUN_TIME;
	};

	pTime = localtime(&currentTime);

	/* Check for error */
	if(pTime == NULL)
	{
		printf("Errors was detected \n");
		return ERR_FUN_TIME;
	}

	printf("Time in California: : %d.%d.%d %d:%d:%d \n", 
	pTime->tm_mday, pTime->tm_mon + 1, pTime->tm_year + 1900, 
	pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

	return 0;
}