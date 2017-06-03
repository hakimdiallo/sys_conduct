#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

int main(){

  struct sockaddr_un my_addr;
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
  int w = write(sfd, "Bonjour", 7);
  if(w < 0){
    perror("write");
    exit(-1);
  }
  close(sfd);

  return 0;

}
