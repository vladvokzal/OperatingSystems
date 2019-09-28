	#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>

#define BUF_SIZE 256

char* read_line(FILE *input){
	int size = 1;
	int length;
	int new_size;
	char buffer[BUF_SIZE];
	char* new_string;
	char *string = (char *)malloc(size);

	if(string == NULL){
		perror("malloc failed");
		return NULL;
	}
	string[0] = '\0';

	while(1){

		if(fgets(buffer, sizeof(buffer), input) == NULL){
			break;
		}

		if(buffer[0] == '\033'){
			printf("ERROR! KEY forbidden, only characters \n");
			return NULL;
		}
		
		length = strlen(buffer);
		
		new_size = size + length;
		new_string = (char *)realloc(string, new_size);

		if(new_string == NULL){
			perror("realloc failed");
			free(string);
			return NULL;
		}
		
		string = new_string;
		size = new_size;
		strcat(string, buffer);
		
		if('\n' == buffer[length - 1]){
			break;
		}
	
	}
	
	return string;
}

int main(int argc, char **argv)
{	
	printf("Pattern: ");

	char *pattern = read_line(stdin);
	
	if(pattern == NULL){
		return -1;
	}

	if('\n' == pattern[strlen(pattern) - 1]){
		pattern[strlen(pattern) - 1] = '\0';
	}

	if(strchr(pattern, '/') != NULL){
		printf("The pattern can't contain '/' character \n");
		return -1;
	}
	
	DIR *directory = opendir(".");

	if(directory == NULL){
		perror("couldn't matched directory");
		return -1;
	}
	
	int matched = 0;

	while(1){
		struct dirent *entry = NULL;
		errno = 0;
		entry = readdir(directory);
		
		if (entry == NULL && (0 == errno)){
			break;
		}

		if (entry == NULL){
			perror("couldn't read directory");
			break;
		}

		struct stat status;
			
		if (stat(entry->d_name, &status) == -1){
			perror("Couldn't get file status");
			break;
		}

		if(S_ISREG(status.st_mode)){
			if(0 == fnmatch(pattern, entry->d_name, 0)){
				printf("%s\n", entry->d_name);
				matched = 1;
			}
		}	
	}

	if(!matched){
		printf("%s\n", pattern);
	}

	closedir(directory);
		
	free(pattern);
	
	return 0;
}
