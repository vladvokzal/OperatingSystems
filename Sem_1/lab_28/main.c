#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <libgen.h> 
#include <stdlib.h>

int main(){
	int receiveValue;
	FILE* fd[2];

	if (p2open("sort -n", fd) == -1){
		perror("p2open error");
		return -1;
	}

	srand(time(NULL));

	for (int i = 0; i < 100; ++i){
		fprintf(fd[0], "%d\n", rand() % 100);
	}
	
	fclose(fd[0]);
	
	for (int i = 0; i < 100; ++i){
		fscanf(fd[1], "%d", &receiveValue);
	
		printf("%d ", receiveValue);
		if ((i % 10) == 0){
			printf("\n");
		}
	}

	fclose(fd[1]);
	
	return 0;
}
