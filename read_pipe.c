#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define MAX_BUF 445

int main()
{
    int fd;
    char * myfifo = "myfifo";
    char buf[MAX_BUF];
    clock_t begin = clock();
    //if (access(myfifo, F_OK) == -1) {
      int s = mkfifo(myfifo, 0666);
      if (s < 0 ) {
        unlink(myfifo);
        perror("not working\n");
        return s;
      }
  //}
    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDONLY );
    read(fd, buf, MAX_BUF);
    clock_t end = clock();
    printf("Received: %s\n", buf);
    close(fd);
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("temps read pipe: %f\n", time_spent);

    return 0;
}
