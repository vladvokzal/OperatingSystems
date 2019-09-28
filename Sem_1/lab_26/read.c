#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>

#define BUF_SIZE 256

int main(){

	char buffer[BUF_SIZE] = { 0 };
	
	setlocale(LC_ALL, "rus");
	scanf("%s", buffer);
	printf("%s \n", buffer);

	return 0;
}
