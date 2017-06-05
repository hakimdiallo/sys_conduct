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
#include <errno.h>
#include "conduct.h"

struct conduct{
  char name[255];
  size_t c;
  size_t a;
  size_t eof;
  size_t current_size;
  pthread_mutex_t verrou;
  pthread_cond_t empty_conduct;
  pthread_cond_t full_conduct;
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;
}conduct;

struct conduct *conduct_create(const char *name, size_t a, size_t c){
  struct conduct* cond;
  if(name == NULL){
    cond =  mmap(NULL, sizeof(struct conduct)+c, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(cond == MAP_FAILED){
      perror("mmap anonyme");
      return NULL;
    }
    cond = (struct conduct *) cond;
    memset(cond, 0, (sizeof(struct conduct)+c));
  }
  else{
    int fd = open(name, O_CREAT |  O_RDWR, 0777);
    if(fd == -1){
      perror("open");
      return NULL;
    }
    int ft = ftruncate(fd, sizeof(struct conduct)+c);
    if(ft == -1){
      perror("ftruncate");
      return NULL;
    }
    cond = mmap(NULL,sizeof(struct conduct)+c, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(cond == MAP_FAILED){
      perror("mmap nomme");
      return NULL;
    }
    close(fd);
    cond = (struct conduct *) cond;
    memset(cond, 0, (sizeof(struct conduct)+c));
    memset(cond->name,0, 255);
    //cond->name = strdup(name);
    strcpy(cond->name, name);
  }
  //memset(cond, 0, (sizeof(struct conduct)+c));
  pthread_mutexattr_init(&cond->mutex_attr);
  pthread_condattr_init(&cond->cond_attr);
  pthread_mutexattr_setpshared(&cond->mutex_attr, PTHREAD_PROCESS_SHARED);
  pthread_condattr_setpshared(&cond->cond_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&cond->verrou, &cond->mutex_attr);
  pthread_cond_init(&cond->empty_conduct,&cond->cond_attr);
  pthread_cond_init(&cond->full_conduct,&cond->cond_attr);
  cond->a = a;
  cond->c = c;
  cond->eof = 0;
  //cond->buffer = (void *) cond + sizeof(struct conduct);
  cond->current_size = 0;
  return cond;
}

struct conduct *conduct_open(const char*name){
  int fd=open(name, O_RDWR, 0777);
  if(fd == -1){
    errno = EPIPE;
    perror("open");
    exit(EXIT_FAILURE);
  }
  struct stat finfo;
  int fs = fstat(fd, &finfo);
  if(fs == -1){
    perror("fstat");
    exit(EXIT_FAILURE);
  }
  struct conduct *cond;
  cond = mmap(NULL,finfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(cond == MAP_FAILED){
    perror("mmap open");
    return NULL;
  }
  cond = (struct conduct *) cond;
  //cond->buffer = (void *)cond + sizeof(struct conduct);
  return cond;
}

ssize_t conduct_read(struct conduct *c, void *buf, size_t count){
  //printf("Thread lecture veut prendre le verrou %zd\n", c->a);
  pthread_mutex_lock(&c->verrou);
  //printf("Thread lecture prend le verrou\n");
    while (c->current_size == 0) {
      if(c->eof == 0){
    //    printf("Thread lecture pas de eof s'endort sur vide\n");
        pthread_cond_wait(&c->empty_conduct, &c->verrou);
    //  printf("Thread lecture se reveille\n");
      }else{
        pthread_mutex_unlock(&c->verrou);
        return 0;
      }
    }
    if (count > c->current_size) {
      count = c->current_size;
    }
    memcpy(buf,(void *)c+sizeof(struct conduct),count);
    if (count == c->current_size) {
      memset((void *)c+sizeof(struct conduct), 0, c->c);
    }
    else{
      memmove((void *)c+sizeof(struct conduct), (void *)c+sizeof(struct conduct)+count, c->current_size - count);
      memset((void *)c+sizeof(struct conduct)+(c->current_size -count ), 0, c->c - (c->current_size - count));
    }
    if (sizeof(c->name) > 0) {
      msync(c, c->c, MS_SYNC | MS_INVALIDATE);
    }
//    printf("%d\n", (int)*c->current_size);
    c->current_size = c->current_size - count;
//  printf("%d\n", (int)*c->current_size);
//    printf("Thread lecture broadcast sur full\n");
  pthread_mutex_unlock(&c->verrou);
  pthread_cond_broadcast(&c->full_conduct);
  //printf("Thread lecture relache le verrou\n");
  return count;
}

ssize_t conduct_write(struct conduct *c, const void *buf, size_t count){
  //printf("Thread ecriture veut prendre le verrou\n");
  pthread_mutex_lock(&c->verrou);
//  printf("Thread ecriture prend le verrou\n");
  while (1) {
    if (c->eof == 1) {
      //printf("Thread ecriture il ya eof\n");
      pthread_mutex_unlock(&c->verrou);
      //printf("Thread ecriture relache le verrou\n");
      errno = EPIPE;
      if(errno == EPIPE){
        perror("eof");
      }
      return -1;
    }
    if (c->current_size == c->c) {
  //    printf("Thread ecriture s'endort sur full\n");
      pthread_cond_wait(&c->full_conduct, &c->verrou);
    //  printf("Thread ecriture se reveille de full\n");
    }
    if ((count <= c->a && count <= (c->c-c->current_size)) || (count > c->a && count <= (c->c-c->current_size))) {
      memcpy((void *)c+sizeof(struct conduct)+c->current_size,buf,count);
      if (sizeof(c->name) > 0) {
        msync(c, c->c, MS_SYNC | MS_INVALIDATE);
      }
      c->current_size = c->current_size  + count;
      //printf("Thread ecriture broadcast sur empty\n");
      pthread_mutex_unlock(&c->verrou);
      pthread_cond_broadcast(&c->empty_conduct);
      //printf("Thread ecriture relache le verrou\n");
      return count;
    }
    else if (count > c->a && count > (c->c-c->current_size)) {
      size_t m = c->c - c->current_size;
      memcpy((void *)c+sizeof(struct conduct)+c->current_size,buf,m);
      if (sizeof(c->name) > 0) {
        msync(c, c->c, MS_SYNC | MS_INVALIDATE);
      }
      c->current_size = c->current_size  + m;
      //printf("Thread ecriture broadcast sur empty\n");
      pthread_mutex_unlock(&c->verrou);
      pthread_cond_broadcast(&c->empty_conduct);
      //printf("Thread ecriture relache le verrou\n");
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
  pthread_mutex_lock(&c->verrou);
  if (c->eof == 0) {
    c->eof = 1;
    pthread_cond_broadcast(&c->empty_conduct);
  }
  pthread_mutex_unlock(&c->verrou);
  return 0;
}

void conduct_close(struct conduct *c){
  if(munmap(c, c->c + sizeof(struct conduct))  == -1){
    perror("failed while unmaping");
    exit(EXIT_FAILURE);
  }
}

void conduct_destroy(struct conduct *c){
  if(sizeof(c->name) > 0){
    if (unlink(c->name) == -1) {
      perror("unlink");
      exit(EXIT_FAILURE);
    }
  }
  conduct_close(c);
}

ssize_t conduct_writev(struct conduct *c, struct iovec *iov, int iovcnt){
  int i;
  size_t size = 0;
  for (i = 0; i < iovcnt; i++) {
    size = size + iov[i].iov_len;
  }
  void *ptr = NULL;
  ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap write v");
    exit(EXIT_FAILURE);
  }
  memset(ptr, 0, size);
  size_t cpt = 0;
  for (i = 0; i < iovcnt; i++) {
    memcpy(ptr+cpt,iov[i].iov_base,iov[i].iov_len);
    cpt = cpt + iov[i].iov_len;
  }
  size_t r = conduct_write(c, ptr, size);
  munmap(ptr, size);
  return r;
}

ssize_t conduct_readv(struct conduct *c, struct iovec *iov, int iovcnt){
  int i;
  size_t size = 0;
  for (i = 0; i < iovcnt; i++) {
    size = size + iov[i].iov_len;
  }
  void *buf = NULL;
  buf = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (buf == MAP_FAILED) {
    perror("mmap read v");
    exit(EXIT_FAILURE);
  }
  memset(buf, 0, size);
  size_t cpt = 0;
  size_t r = conduct_read(c, buf, size);
  i = 0;
  while (cpt < r || i < iovcnt) {
    memcpy(iov[i].iov_base,(void *)buf+cpt,iov[i].iov_len);
    cpt = cpt + iov[i].iov_len;
    i++;
  }
  munmap(buf, size);
  return r;
}
