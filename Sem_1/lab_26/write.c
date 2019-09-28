#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>

#define BUF_SIZE 256

int main(){
	
	setlocale(LC_ALL, "rus");
	
	char buffer[BUF_SIZE] = { 0 };
	scanf("%s", buffer);
	
	for (int i = 0; i < strlen(buffer); i++) {
		buffer[i] = toupper(buffer[i]);//русские буквы, нужно чтобы работало
	}
		
	printf("After toupper: %s\n", buffer);
	
	return 0;
}


		