#include "conduct.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  struct conduct *cond;
  cond = conduct_create(NULL, 20, 40);
  char buff[] = "Salam";
  int f = fork();
  if(f == 0){
    int rc1 = conduct_write(cond,buff, strlen(buff));
    printf("%d\n",rc1 );
    if(rc1 < 0){
      perror("conduct_write :");
      exit(-1);
    }
    conduct_close(cond);
    exit(1);
  }else{
  wait(NULL);
  int rc2 = conduct_read(cond, buff, 10);
  printf("%d\n", rc2);
  if(rc2 < 0){
    perror("conduct_read :");
    exit(-1);
  }
  printf("%s\n", buff);
  conduct_close(cond);
  return 0;
  }
}
