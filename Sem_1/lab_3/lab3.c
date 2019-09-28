#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char * argv[] ) {
    
    uid_t userId = getuid();
    uid_t effectiveId = geteuid();

    printf("User ID %d \n", userId);
    printf("Effective user ID %d \n", effectiveId);

    setuid(1111);

    printf("New User ID: %d\n", getuid());    

    if( argc != 2 ){
        return -1;
    }

    FILE* fp = fopen(argv[1], "rw" );
    
    if( ! fp ) {
        perror("open error\n");
        return -1;
    }   

    fclose(fp);

    return 0;
}
