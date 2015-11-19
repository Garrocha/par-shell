#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

/* Initiates a semaphore and tests if it was successfully initiated. */
void Sem_init(sem_t *sem, int val){
  int err = 0;
  err = sem_init(sem,0,val);
  if (err != 0) {
    fprintf(stderr,"Error on sem_init: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Destroys a semaphore and tests if it was successfully destroyed. */
void Sem_destroy(sem_t *sem){
  int err = 0;
  err = sem_destroy(sem);
  if (err != 0) {
    fprintf(stderr,"Error on sem_destroy: %s\n",strerror(errno));
  }
}

/* Post -> semaphor, that is, increments its value or unblock a process. */
void Sem_post(sem_t *sem){
  int err = 0;
  err = sem_post(sem);
  if (err != 0)
    fprintf(stderr,"Error on sem_post: %s\n",strerror(errno));
}

/* Wait -> semaphor, that is, decrements its value or block a process. */
void Sem_wait(sem_t *sem){
  int err = 0;
  err = sem_wait(sem);
  if (err != 0) {
    fprintf(stderr,"Error on sem_destroy: %s\n",strerror(errno));
  }
}
