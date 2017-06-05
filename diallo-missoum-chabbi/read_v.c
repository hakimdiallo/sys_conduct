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
  iov[0].iov_len = sizeof("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ");
  iov[0].iov_base = malloc(iov[0].iov_len);
  iov[1].iov_len = sizeof("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ");
  iov[1].iov_base = malloc(iov[1].iov_len);
  iov[2].iov_len = sizeof("Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. ");
  iov[2].iov_base = malloc(iov[2].iov_len);
  cond = conduct_create("tatav", 445, 1024+1024);
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
