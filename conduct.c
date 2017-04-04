#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "conduct.h"

struct conduct{
  size_t *c;
  size_t *a;
  int *addr;
}conduct;

struct conduct *conduct_create(const char *name, size_t a, size_t c){
struct conduct* cond= (struct conduct*) malloc(sizeof(conduct));
cond->a=(size_t*)malloc(sizeof(size_t));
cond->c=(size_t*)malloc(sizeof(size_t));
*cond->a=a;
*cond->c=c;
if(name==NULL){
cond->addr=(int*)mmap(NULL, c, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if(cond->addr==MAP_FAILED){
    perror("conduct_create failed");
    exit(-1);
  }
}
else{
  int fd=open(name, O_CREAT | O_RDWR, 0666);
  if(fd==NULL){
    perror("no file descriptor error open");
    exit(-1);
  }
  cond->addr=(int*)mmap(NULL, c, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(cond->addr == MAP_FAILED){
    perror("erreur map failed");
    exit(-1);
  }
  char b[1000];
  if(sprintf(b, "%p",cond)<0){
    perror("failed to print");
    exit(-1);
  }
  if(write(fd, b, strlen(b))==-1){
    perror("failed to write");
    exit(-1);
  }

  if(close(fd)==-1){
    perror("failed to close file");
    exit(-1);
  }
}
return cond;
}

struct conduct *conduct_open(const char*name){
int fd=open(name, O_RDONLY);
if(fd==-1){
  perror("failed to open the file");
  exit(-1);
}
struct conduct *cond=(struct conduct*)malloc(sizeof(struct conduct));
char *b=(char *)malloc(1000*sizeof(char));
if(read(fd, b, strlen(b))==-1){
  perror("failed to read the file");
  exit(-1);
}
if(close(fd)==-1){
  perror("failed to close the file");
  exit(-1);
}
cond=(struct conduct*)b;
return cond;
}

ssize_t conduct_read(struct conduct *c, void *buf, size_t count){

}

ssize_t conduct_write(struct conduct *c, const void *buf, size_t count){

}

int conduct_write_eof(struct conduct *c){

}

void conduct_close(struct conduct *conduct){

}

void conduct_destroy(struct conduct *conduct){

}
