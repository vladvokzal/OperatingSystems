#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <glob.h>
 

void print_matched_files(char *mask);
 
int main(int argc, char **argv) {
	
	for (int i = 1; i < argc; ++i) {
		print_matched_files(argv[i]);
	}
	
 
	return 0;
}


int errFunc(const char *epath, int errno) {
	fprintf(stderr, "Can't check for matching: %s\n", epath);
	return -1;
}
 
void print_matched_files(char *mask) {
	glob_t glob_results;
 
	int glob_status = glob(mask, GLOB_NOCHECK, errFunc, &glob_results);
	
	if (glob_status == GLOB_ABORTED) {
		exit(0);
	}
 
	int matching_index = 0;
 
	while (glob_results.gl_pathv[matching_index] != NULL) {
		printf("%s ", glob_results.gl_pathv[matching_index]);
		matching_index++;
	}
 
	if (matching_index) {
		printf("\n");
	}
 
	globfree(&glob_results);
}