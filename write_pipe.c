#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int fd;
    char * myfifo = "myfifo";

    /* create the FIFO (named pipe) */
    clock_t begin = clock();

    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
      perror("open \n");
      return fd;
    }
    int w = write(fd, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", sizeof("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."));
    clock_t end = clock();
    close(fd);
    printf("%d\n", w);

    /* remove the FIFO */
    unlink(myfifo);
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("temps write pipe: %f\n", time_spent);

    return 0;
}
