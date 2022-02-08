#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#define BUF_SIZE 4096

int main()
{
    char buffer[BUF_SIZE];
    int fd, ret, id, grade[5];
    memset(grade, 0, sizeof(grade));
    ssize_t numOut;
    /*
    O_WRONLY: Write only
    O_APPEND: Append to the end of grade.txt
    O_CREATE: If the filename doesn't exist, create it
    S_IRUSR, S_IWUSR: Only owner can read/write the file.= 
    */
    fd = open("grade.txt", O_WRONLY|O_APPEND|O_CREAT, S_IRUSR| S_IWUSR);	
    while (1) {
        // exclusive-or | non-blocking lock
        if (flock(fd, LOCK_EX | LOCK_NB) == 0)
            printf("Enter critical section\n");
        // input the data
        scanf("%d%d%d%d%d%d", &id, &grade[0], &grade[1], &grade[2], &grade[3], &grade[4]);
        // print to the buffer
        sprintf(buffer, "%d %d %d %d %d %d\n", id, grade[0], grade[1], grade[2], grade[3], grade[4]);
        if (fd < 0) {
            perror("File open failed"); 
            exit(-1);
	}
        if ((numOut = write(fd, buffer, strlen(buffer))) < 0) {
            perror("Buffer write failed");
            exit(-1);
        }
	sleep(5);
	printf("Exit critical section\n");
        // unlock
        flock(fd, LOCK_UN);
    }		
    close(fd);
    return 0;
}
