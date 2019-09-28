#include <stdio.h>
#include <limits.h>

#include "list.h"

#define END_READ '.'
#define ERR_READ -1


int main(int argc, char **argv)
{
	List* stringsList = NULL;
	char string[LINE_MAX] = {0};
	int endOfRead = 0;

	while(END_READ != string[0]) 
	{	

		fgets(string, LINE_MAX, stdin);

		if(string[0] == '\033')
		{
			printf("ERROR! KEY forbidden, only characters \n");
			return -1;
		}

		if (END_READ != string[0]) 
		{

			endOfRead = insertList(&stringsList, string);
			
			if(!endOfRead)
			{
				freeList(&stringsList);
				perror("ERROR");
				return ERR_READ;
			}
		}

	}

	printf("\n");
	printList(stringsList);
	freeList(&stringsList);

	return 0;
}