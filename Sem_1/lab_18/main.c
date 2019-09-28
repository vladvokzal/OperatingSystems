#include <stdlib.h> 
#include <stdio.h>
#include <time.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <pwd.h> 
#include <string.h>
#include <grp.h> 
#include <unistd.h>
#include <libgen.h>


void printFileType(struct stat status) {
	if (S_ISDIR(status.st_mode)) {
		printf("d");
	}
	else if (S_ISREG(status.st_mode)) {
		printf("-");
	}
	else {
		printf("?");
	}
	
}

void printFilePermission(struct stat status){
	printf("%c", (status.st_mode & S_IRUSR) ? 'r' : '-');
	printf("%c", (status.st_mode & S_IWUSR) ? 'w' : '-');
	printf("%c", (status.st_mode & S_IXUSR) ? 'x' : '-');

	printf("%c", (status.st_mode & S_IRGRP) ? 'r' : '-');
	printf("%c", (status.st_mode & S_IWGRP) ? 'w' : '-');
	printf("%c", (status.st_mode & S_IXGRP) ? 'x' : '-');

	printf("%c", (status.st_mode & S_IROTH) ? 'r' : '-');
	printf("%c", (status.st_mode & S_IWOTH) ? 'w' : '-');
	printf("%c", (status.st_mode & S_IXOTH) ? 'x' : '-');
	
	printf("   ");
	
}

void printFileNameUID(struct passwd *password){
	printf("%s", password->pw_name);
	printf(" ");	
}

void printFileNameGID(struct group *group){
	printf(" %s", group->gr_name);
	printf(" ");
}

void printFileSize(struct stat status){
	int curFieldSize = 0;
	
	if (S_ISREG(status.st_mode)) {
		printf(" %ld", status.st_size);
		for (int i = status.st_size; i != 0; ++curFieldSize , i /= 10);
	}
 	
 	printf(" ");
}

void printFileLastRecordTime(struct stat status){
	char *lastRecordTime = ctime(&status.st_mtime);

	lastRecordTime[strlen(lastRecordTime) - 1] = 0;
	printf("%s", lastRecordTime);
	printf(" ");
}

void printFileBaseName(char *name){
	printf("%s", basename(name));
	
	printf("\n");
	
}

void startPrintFileInfo(struct stat currentFileStatus, struct passwd *password, struct group *group, char *name){
	printFileType(currentFileStatus);
	printFilePermission(currentFileStatus);
	printFileNameUID(password);
	printFileNameGID(group);
	printFileSize(currentFileStatus);
	printFileLastRecordTime(currentFileStatus);
	printFileBaseName(name);
}

int main(int argc, char **argv) {
	
	if(argc < 2){
		printf("Usage: %s [file_name]\n", argv[0]);
		return -1;
	}

	for (int i = 1; i < argc; i++){

		struct stat currentFileStatus;
		
		if (stat(argv[i], &currentFileStatus) == -1){
			perror("stat error");
			continue;
		}

		struct passwd *password = getpwuid(currentFileStatus.st_uid);
		if (password == NULL) {
			perror("getpwuid error");
			continue;
		}

		struct group *group = getgrgid(currentFileStatus.st_gid);
		if (group == NULL){
			perror("getgrgid error");
			continue;
		}

		startPrintFileInfo(currentFileStatus, password, group, argv[i]);
	}

	return 0;
}


//почему /*/*/*/*/*/ не сработало, хотя есть вложенности более глубокого уровня

