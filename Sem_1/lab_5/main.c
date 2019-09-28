#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define SIZE 1024
#define LINES 100
#define BAD_ARG -1
#define ERR_OPEN -2
#define ERR_CLOSE -3


struct lineInfo
{
	int shift;
	int length;
};

int checkErrLine(int lineNumber, int stringCounter)
{
	return (lineNumber < 0) || (lineNumber > stringCounter);
}

int main(int argc, char** argv)
{
	char buffer[SIZE];
	struct lineInfo table[LINES];

	int line;
	int file;
	int length = 0;
	int stringcounter = 0;
	int prevshift = 0;
	int finalShift = 0;
	
	ssize_t readedBytes;

	if((file = open(argv[1], O_RDONLY)) < 0)
	{
		perror("open");
		return ERR_OPEN;
	}

	while((readedBytes = read(file, buffer, SIZE)) && stringcounter <= 100)
	{
	
		for(int i = 0; i < readedBytes; ++i)
		{

			if(buffer[i] == ' ')
				prevshift++;
			
			table[stringcounter].shift = prevshift;

			if(buffer[i] == '\n')
			{
				stringcounter++;
				prevshift = 0;
				length = 0;
			}

			++length;

			table[stringcounter].length = length;

		}

	}
	

	printf("LineNumber: Shift: Length: \n");

	for (int i = 0; i < stringcounter; ++i)
	{
		printf("%d \t\t %d\t%d \n", i + 1, table[i].shift, table[i].length);
	}
	
	printf("Enter number of string from 0 to %d: ", stringcounter - 1);
	
	if (scanf("%d",&line) == 0)
	{
		printf("Incorrect input \n");
		return BAD_ARG;
	}

	if(checkErrLine(line, stringcounter - 1))
	{
		printf("LineNumber doesn't exist \n");
		return BAD_ARG;
	}

	for (int i = 0; i < line; ++i)
	{
		finalShift += table[i].length;
	}
	
	printf("Shift: %d\n", finalShift); 

	lseek(file, finalShift, SEEK_SET);	
	read(file, buffer, table[line].length);

	buffer[table[line].length] = '\0';

	printf("Your str: %s\n", buffer);
	
	if( close(file) < 0)
	{
		perror("exit");
		return ERR_CLOSE;
	}
	
	return 0;
}
