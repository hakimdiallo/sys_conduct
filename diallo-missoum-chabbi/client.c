#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(){

  struct sockaddr_un my_addr;
  clock_t begin = clock();
  int sfd=socket(AF_UNIX,SOCK_STREAM,0);
  if(sfd == -1){
    perror("socket");
    exit(-1);
  }
  my_addr.sun_family = AF_UNIX;
  strcpy(my_addr.sun_path,"socket");
  if(connect(sfd, (struct sockaddr *) &my_addr,  sizeof(struct sockaddr_un)) == -1){
    perror("connect");
    exit(-1);
  }
  int w = write(sfd, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", 445);
  clock_t end = clock();
  if(w < 0){
    perror("write");
    exit(-1);
  }
  close(sfd);
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("temps write conduct: %f\n", time_spent);
  return 0;

}
