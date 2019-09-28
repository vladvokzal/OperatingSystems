#include <ctype.h>
#include <string.h>
#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#define BUF_SIZE 256

int main() {

	setlocale(LC_ALL, "rus");
	FILE* receivePipe = NULL;
	FILE* sendPipe = NULL;
	char buffer[BUF_SIZE] = { 0 };

	receivePipe = popen("./read", "r");
	if (NULL == receivePipe) {
		perror("popen error");
		return -1;
	}
	
	sendPipe = popen("./write", "w");
	if (NULL == receivePipe) {
		perror("popen error");
		return -1;
	}

	if (read(fileno(receivePipe), buffer, BUF_SIZE) == -1) {
		perror("read error");
		return -1;
	}
	
	if (write(fileno(sendPipe), buffer, strlen(buffer)) == -1) {
		perror("read error");
		return -1;
	}
	
	//почему здесь нельзя сделать fclose? чем отличается
	//pclose закрывает дексриптор только после того, как завершатся все процессы, ассоциируемые с данным канлом
	//pclose() == wait() + fclose(), как он понимает что нужно закрыть канал, если им владеют много процессов,
	//каждому делать пклоус не нужно, один сделает и все будет работать

	pclose(receivePipe);
	pclose(sendPipe);

	return 0;
}
