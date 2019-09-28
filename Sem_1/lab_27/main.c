#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

int main(int argc, char* argv[]) {
	
	FILE* file = NULL;
	FILE* sendPipe = NULL;

	if (2 != argc){
		printf("Usage: %s [file_name] \n", argv[0]);
		return -1;
	}

	file = fopen(argv[1], "r");

	if (NULL == file){
		perror("fopen error");
		return -1;
	}

	//как сделать механизм общения между неродственными процессами процессами? (такой механизм есть)
	//here use command wc to count new lines
	sendPipe = popen("wc -l", "w");

	if (NULL == sendPipe){
		perror("popen error");
		return -1;
	}

	char previousCharFromFile = '\n';
	char charFromFile = fgetc(file);

	while (EOF != charFromFile){

		if(('\n' == charFromFile) && ('\n' == previousCharFromFile)){
			fputc('\n', sendPipe);
		}
		previousCharFromFile = charFromFile;
		charFromFile = fgetc(file);
	}

	fclose(file);
	pclose(sendPipe);

	return 0;
}

