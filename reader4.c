#include "conduct.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>


void * reader(void *arg){
  struct conduct *cond;
  cond = conduct_open("tata");
  //cond = conduct_create("tata", 20, 40);
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

int main(int argc, char const *argv[]) {
  pthread_t th;

  int r = pthread_create(&th, NULL, reader, NULL);
  pthread_join(th,NULL);
  return 0;
}
