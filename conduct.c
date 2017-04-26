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
  char *name;
  size_t c;
  size_t a;
  size_t eof;
  size_t current_size;
  pthread_mutex_t verrou;
  pthread_cond_t empty_conduct;
  pthread_cond_t full_conduct;
  pthread_cond_t not_enough_place;
  void *buffer;
}conduct;

struct conduct *conduct_create(const char *name, size_t a, size_t c){
  struct conduct* cond;
  if(name==NULL){
    cond = (struct conduct *) mmap(NULL, sizeof(struct conduct)+c, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(cond == MAP_FAILED){
      perror("conduct_create failed");
      exit(-1);
    }
  }
  else{
    int fd=open(name, O_CREAT |  O_RDWR, 0777);
    if(fd == -1){
      perror("no file descriptor error open");
      exit(-1);
    }
    int ft = ftruncate(fd, sizeof(struct conduct)+c);
    if(ft == -1){
      perror("ftruncate");
      exit(-1);
    }
    cond = mmap(NULL,sizeof(struct conduct)+c, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(cond == MAP_FAILED){
      perror("erreur map failed");
      exit(-1);
    }
    close(fd);
    cond->name=strdup(name);


  }
  memset(cond, 0, (sizeof(struct conduct)+c));

  pthread_mutex_init(&cond->verrou, NULL);
  pthread_cond_init(&cond->empty_conduct,NULL);
  pthread_cond_init(&cond->full_conduct,NULL);
  pthread_cond_init(&cond->not_enough_place,NULL);
  cond->a=a;
  cond->c=c;
  cond->eof = 0;
  cond->buffer = (void *) cond + sizeof(struct conduct);
  //cond->current_size = (size_t *)malloc(sizeof(size_t));
  cond->current_size = 0;
  //memset(cond+sizeof(struct conduct)+c, 0, cond->c);
  return cond;
}

struct conduct *conduct_open(const char*name){
  int fd=open(name, O_RDWR, 0777);
  if(fd == -1){
    perror("failed to open the file");
    exit(-1);
  }
  struct stat finfo;
  int fs = fstat(fd, &finfo);
  if(fs == -1){
    perror("failed to fstat");
    exit(-1);
  }
  struct conduct *cond;
  cond = mmap(NULL,finfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(cond == MAP_FAILED){
    perror("erreur map failed");
    exit(-1);
  }
  cond->buffer = (void *)cond + sizeof(struct conduct);
  return cond;
}

ssize_t conduct_read(struct conduct *c, void *buf, size_t count){
  printf("Thread lecture veut prendre le verrou %zd\n", c->a);
  pthread_mutex_lock(&c->verrou);
  //printf("Thread lecture prend le verrou\n");
    while (c->current_size == 0) {
      if(c->eof == 0){
    //    printf("Thread lecture pas de eof s'endort sur vide\n");
        pthread_cond_wait(&c->empty_conduct, &c->verrou);
    //  printf("Thread lecture se reveille\n");
      }else{
        return 0;
      }
    }
    size_t m = minimum(c->current_size, count);
    memcpy(buf,c->buffer,m);
    if (m == c->current_size) {
      memset(c->buffer, 0, c->c);
    }
    else{
      /*void *tmp = 0;// = (void *)malloc(*c->c*sizeof(void));
      memset(tmp, 0, c->c);
      memcpy(tmp, c+(sizeof(struct conduct *))+m, c->current_size - m);
      memset(c+(sizeof(struct conduct *)), 0, c->c);
      memcpy(c+(sizeof(struct conduct *)), tmp, c->current_size - m);*/
      memmove(c->buffer, c->buffer+m, c->current_size - m);
      memset(c->buffer+(c->current_size -m ), 0, c->c - (c->current_size - m));
    }
//    printf("%d\n", (int)*c->current_size);
    c->current_size = c->current_size - m;
//  printf("%d\n", (int)*c->current_size);
//    printf("Thread lecture broadcast sur full\n");
    pthread_cond_broadcast(&c->not_enough_place);
    pthread_cond_broadcast(&c->full_conduct);
  pthread_mutex_unlock(&c->verrou);
  printf("Thread lecture relache le verrou\n");
  return m;
}

ssize_t conduct_write(struct conduct *c, const void *buf, size_t count){
  printf("Thread ecriture veut prendre le verrou\n");
  pthread_mutex_lock(&c->verrou);
//  printf("Thread ecriture prend le verrou\n");
  while (1) {
    if (c->eof == 1) {
      // errno
      //printf("Thread ecriture il ya eof\n");
      pthread_mutex_unlock(&c->verrou);
      //printf("Thread ecriture relache le verrou\n");
      return -1;
    }
    if (c->current_size == c->c) {
  //    printf("Thread ecriture s'endort sur full\n");
      //pthread_cond_broadcast(c->empty_conduct);
      pthread_cond_wait(&c->full_conduct, &c->verrou);
    //  printf("Thread ecriture se reveille de full\n");
    }
    if ((count <= c->a && count <= (c->c-c->current_size)) || (count > c->a && count <= (c->c-c->current_size))) {
      memcpy(c->buffer+c->current_size,buf,count);
      c->current_size = c->current_size  + count;
      //printf("Thread ecriture broadcast sur empty\n");
      pthread_cond_broadcast(&c->empty_conduct);
      pthread_mutex_unlock(&c->verrou);
      printf("Thread ecriture relache le verrou\n");
      return count;
    }
    else if (count > c->a && count > (c->c-c->current_size)) {
      size_t m = c->c - c->current_size;
      memcpy(c->buffer+c->current_size,buf,m);
      c->current_size = c->current_size  + m;
      //printf("Thread ecriture broadcast sur empty\n");
      pthread_cond_broadcast(&c->empty_conduct);
      pthread_mutex_unlock(&c->verrou);
      printf("Thread ecriture relache le verrou\n");
      return m;
    }
    else {
      //printf("Thread ecriture s'endort sur not_enough_place\n");
      //pthread_cond_broadcast(c->empty_conduct);
      pthread_cond_wait(&c->full_conduct, &c->verrou);
      //printf("Thread ecriture se reveille de not_enough_place\n");
    }
  }
}

int conduct_write_eof(struct conduct *c){
  if (c->eof == 0) {
    c->eof = 1;
    pthread_cond_broadcast(&c->empty_conduct);
  }
  return 0;
}

void conduct_close(struct conduct *conduct){
  if(conduct->name == NULL ){
    if(munmap(conduct, conduct->c + sizeof(struct conduct))  == -1){
      perror("failed while unmaping");
      exit(-1);
    }
  }
  if(munmap(conduct, conduct->c + sizeof(struct conduct))  == -1){
    perror("failed while unmaping");
    exit(-1);
  }
}

void conduct_destroy(struct conduct *conduct){
  if(munmap(conduct->buffer, conduct->c) == -1){
    perror("failed while unmaping");
    exit(-1);
  }
  //free(conduct->a);
  //free(conduct->c);
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
