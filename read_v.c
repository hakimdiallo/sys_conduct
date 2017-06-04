#include "conduct.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define N 3
int main(){
  struct conduct *cond;
  struct iovec *iov = malloc(N * sizeof(struct iovec));

  cond = conduct_create("tata", 445, 1024);
  //printf("%s\n",cond->name);
  //cond = conduct_open("tata");
  int rc1 = conduct_readv(cond, iov, N);
  printf("%d\n", rc1);
  if(rc1 < 0){
    perror("conduct_read :");
    exit(-1);
  }
  int i;
  for (i = 0; i < N; i++) {
    printf("%s\n", (char *)iov[i].iov_base);
  }
  return 0;
}
