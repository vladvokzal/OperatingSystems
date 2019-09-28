#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define SIZE 1024
#define LINES 100
#define BAD_ARG -1
#define ERR_OPEN -2
#define ERR_CLOSE -3

int descriptor = 0;

struct lineInfo
{
	int shift;
	int length;
};

void checkErrLine(int lineNumber, int stringCounter)
{
	if((lineNumber < 0) || (lineNumber > stringCounter))
	{
		printf("LineNumber doesn't exist \n");
	}
}

int printFile( int descriptor ) 
{
	char buffer[SIZE + 1] = { 0 };
	
	if ( lseek( descriptor, 0, SEEK_SET ) < 0 ) 
	{
		perror("lseak error");
		return 0;
	}
	ssize_t readSymbol = read( descriptor, buffer, SIZE );
	
	while ( 0 != readSymbol) 
	{
		if ( -1 == readSymbol ) 
		{
			perror("read error");
			return 0;
		}
		buffer[readSymbol] = '\n';
		printf("%s", buffer);
		readSymbol = read( descriptor, buffer, SIZE );
	}

	return 1;
}


void alarmSignalCatcher(int signalId) 
{
	printf("\n 5 seconds passed: \n");

	if ( !printFile( descriptor) )
	{
		exit(1);
	}

	if ( close(descriptor) < 0 ) 
	{
		perror("close error");
		exit(1);
	}
	exit(0);
}


int main(int argc, char** argv)
{
	char buffer[SIZE];
	struct lineInfo table[LINES];

	int line;
	int file;
	int size;
	int length = 0;
	int stringcounter = 0;
	int prevshift = 0;
	int finalShift = 0;
	char* mappingDAta;
	
	ssize_t readedBytes;

	if((file = open(argv[1], O_RDONLY)) < 0)
	{
		perror("open");
		return ERR_OPEN;
	}

	size = lseek(file, 0, SEEK_END);
	
	/* Создается новое отображение в виртуальном адресном пространстве вызывающего процесса
	
	void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
    
    */
	mappingDAta = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, file, 0);
	
	if(mappingDAta == MAP_FAILED){
		printf("Error in mapping \n");
		return -1;
	}

	descriptor = file;
	
	for(int i = 0; i < size && stringcounter <= 100; ++i)
	{
	
		if(mappingDAta[i] == ' ')
			prevshift++;
		
		table[stringcounter].shift = prevshift;

		if(mappingDAta[i] == '\n')
		{
			stringcounter++;
			prevshift = 0;
			length = 0;
		}

		++length;

		table[stringcounter].length = length;

	}
	
	printf("LineNumber: Shift: Length: \n");

	for (int i = 0; i < stringcounter; ++i)
	{
		printf("%d \t\t %d\t%d \n", i + 1, table[i].shift, table[i].length);
	}
	
	//Обработка сигнала	
	if ( SIG_ERR == signal(SIGALRM, alarmSignalCatcher))
	{
		perror("Cann't set signal catcher");
		return EXIT_FAILURE;
	}
	
	printf("Enter number of string from 0 to %d: ", stringcounter - 1);
	
	//Устанавливаем время вызываемому процессу в 5 секунд
	alarm(5);

	if (scanf("%d",&line) == 0) 
	{
		//Ошибка - отменяется ожидающий аварийный сигнал, так как передали 0
		alarm(0);
		while ( '\n' != getchar() );
		printf("Incorrect input \n");
		return BAD_ARG;
	}
	
	//Сбрасываем ожиающий сигнал
	alarm(0);
	
	if ( '\n' != getchar() ) 
	{
		printf("Invalid argument \n");
		while ( '\n' != getchar());
		return 0;
	}

	checkErrLine(line, stringcounter - 1);

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
	
	munmap(mappingDAta, size);
	return 0;
}
