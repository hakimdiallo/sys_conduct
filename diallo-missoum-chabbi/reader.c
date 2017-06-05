#include "conduct.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


int main(){
  struct conduct *cond;
  char buff[445];
  cond = conduct_create("tata", 445, 1024);
  clock_t begin = clock();
  //printf("%s\n",cond->name);
  //cond = conduct_open("tata");
  int rc1 = conduct_read(cond, buff, 445);
  clock_t end = clock();
  printf("%d\n", rc1);
  if(rc1 < 0){
    perror("conduct_read :");
    exit(-1);
  }
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("temps read conduct: %f\n", time_spent);
  printf("%s\n", buff);
  conduct_destroy(cond);
  conduct_open("tata");
  return 0;
}
