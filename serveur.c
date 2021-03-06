#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>

#define NB_MAX 15


int main(){

  unlink("socket");
  struct sockaddr_un my_addr;
//  socklen_t peer_addr_size;
  char buff[445];
  my_addr.sun_family = AF_UNIX;
  strcpy(my_addr.sun_path,"socket");
  clock_t begin = clock();
  int sfd=socket(AF_UNIX,SOCK_STREAM,0);
  if(sfd == -1){
    perror("socket");
    exit(-1);
  }
  if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1){
         perror("bind");
         exit(-1);
}
 if(listen(sfd, NB_MAX) == -1){
     perror("listen");
     exit(-1);
 }
 int cfd;
 while(1){
     cfd = accept(sfd, 0,0);
     if (cfd == -1){
       perror("accept");
       exit(-1);
     }
     int r = read(cfd, buff, 445);
     clock_t end = clock();
     if(r < 0){
       perror("read");
       exit(-1);
     }
     double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
     printf("temps write conduct: %f\n", time_spent);
     printf("%s\n", buff);
  }

 close(cfd);
 close(sfd);

 return 0;

 }
