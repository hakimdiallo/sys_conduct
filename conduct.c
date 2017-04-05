#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "conduct.h"

struct conduct{
  char * name;
  size_t *c;
  size_t *a;
  void *addr;
  int *cpt;
  int *eof;
  pthread_mutex_t *verrou;
  pthread_cond_t *empty_conduct;
  pthread_cond_t *full_conduct;
}conduct;

struct conduct *conduct_create(const char *name, size_t a, size_t c){
  struct conduct* cond= (struct conduct*) malloc(sizeof(conduct));
  cond->a=(size_t*)malloc(sizeof(size_t));
  cond->c=(size_t*)malloc(sizeof(size_t));
  *cond->a=a;
  *cond->c=c;
  cond->eof = (int *)malloc(sizeof(int));
  *cond->eof = 0;
  pthread_mutex_init(cond->verrou,NULL);
  pthread_cond_init(cond->empty_conduct,NULL);
  pthread_cond_init(cond->full_conduct,NULL);
  if(name==NULL){
    cond->addr = mmap(NULL, c, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(cond->addr==MAP_FAILED){
      perror("conduct_create failed");
      exit(-1);
    }
  }
  else{
    cond->name=strdup(name);
    int fd=open(name, O_CREAT | O_RDWR, 0666);
    if(fd == -1){
      perror("no file descriptor error open");
      exit(-1);
    }
    cond->addr = mmap(NULL, c, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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
  if(fd == -1){
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
  cond = (struct conduct*)b;
  return cond;
}

ssize_t conduct_read(struct conduct *c, void *buf, size_t count){
  pthread_mutex_lock(c->verrou);
    while (*c->a == 0) {
      if(*c->eof == 0){
        pthread_cond_wait(c->empty_conduct, c->verrou);
      }else{
        return 0;
      }
    }
    size_t m = minimum(*c->a, count);
    memcpy(buf,c->addr,m);
    *c->a = *c->a - m;
    pthread_cond_broadcast(c->full_conduct);
  pthread_mutex_lock(c->verrou);
  return m;
}

ssize_t conduct_write(struct conduct *c, const void *buf, size_t count){
  pthread_mutex_lock(c->verrou);
    if (*c->a == *c->c) {
        pthread_cond_wait(c->full_conduct, c->verrou);
    }
    if (*c->eof == 1) {

      return -1;
    }
    size_t m = 0;
    if (count <= (*c->c-*c->a)) {
      m = count;
    }
    else{
      m = *c->c - *c->a;
    }
    memcpy(c->addr,buf,m);
    *c->a = *c->a  + m;
    pthread_cond_broadcast(c->empty_conduct);
  pthread_mutex_lock(c->verrou);
  return m;
}

int conduct_write_eof(struct conduct *c){
  if (*c->eof == 0) {
    *c->eof = 1;
  }
  return 0;
}

void conduct_close(struct conduct *conduct){
  free(conduct);
}

void conduct_destroy(struct conduct *conduct){
  if(munmap(conduct->addr, *conduct->c) == -1){
    perror("failed while unmaping");
    exit(-1);
  }
  free(conduct->a);
  free(conduct->c);
  if(conduct->name == NULL){
    if (unlink(conduct->name) == -1) {
      perror("unlink failed");
      exit(-1);
    }
    free(conduct->name);
  }
  conduct_close(conduct);
}

size_t minimum(size_t n, size_t c){
  if (n > c) {
    return c;
  }
  return n;
}
