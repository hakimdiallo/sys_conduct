#include "conduct.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>


int main(){
  struct conduct *cond;
  cond = conduct_open("tata");
  char buff[10];
  int rc1 = conduct_read(cond, buff, 10);
  printf("%d\n", rc1);
  if(rc1 < 0){
    perror("conduct_read :");
    exit(-1);
  }
  printf("%s\n", buff);
  return 0;


}
