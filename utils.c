#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>  /* kill */
#include <sys/stat.h>   /* mkfifo */
#include <fcntl.h>      /* open */
#include <signal.h>     /* signal, kill */
#include <pthread.h>

#include "utils.h"
#include "list.h"

#define MAX_STRING 256

/* Initialites a mutex and tests if it was successfully initialized */
void mutexInit(pthread_mutex_t *mutex){
  if (pthread_mutex_init (mutex,NULL) != 0) {
    fprintf(stderr,"Error on pthread_mutex_init: %s\n",strerror(errno));
  }
}

/* Destroy a mutex and tests if it was successfully destroyed. */
void destroyMutex(pthread_mutex_t *mutex){
  if (pthread_mutex_destroy(mutex) != 0)
    fprintf(stderr,"Error on mutex_destroy: %s\n",strerror(errno));
}

/* Locks mutex and tests if ocurred any errors. */
void mutexLock(pthread_mutex_t *mutex){
  if (pthread_mutex_lock(mutex) != 0) {
    fprintf(stderr,"Error on pthread_mutex_lock: %s\n",strerror(errno));
  }
}

/* Unlocks mutex and tests if ocurred any errors. */
void mutexUnlock(pthread_mutex_t *mutex){
  if (pthread_mutex_unlock(mutex) != 0) {
    fprintf(stderr,"Error on pthread_mutex_unlock: %s\n",strerror(errno));
  }
}

/* Destroy a mutex and tests if it was successfully destroyed. */
void destroy_mutex(pthread_mutex_t *mutex){
  if (pthread_mutex_destroy(mutex) != 0)
    fprintf(stderr,"Error on mutex_destroy: %s\n",strerror(errno));
}

/* Initialize the condition variable referenced by cond with attributes
   referenced by attr. Also verifies if the function was successful. */
void Cond_init(pthread_cond_t *cond, const pthread_condattr_t *restrict attr){
  if (pthread_cond_init(cond, attr) != 0)
    fprintf(stderr,"Error on pthread_cond_init: %s\n",strerror(errno));
}

/* Destroys the given condition variable specified by cond.
   Verifies if the function was successful. */
void Cond_destroy(pthread_cond_t *cond){
  if (pthread_cond_destroy(cond) != 0)
    fprintf(stderr,"Error on pthread_cond_destroy: %s\n",strerror(errno));
}

/* Atomically release mutex and cause the calling thread to block on the
   condition variable cond. Verifies if the function was successful. */
void Cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
  if (pthread_cond_wait(cond, mutex) != 0)
    fprintf(stderr,"Error on pthread_cond_wait: %s\n",strerror(errno));
}

/* Unblocks at least one of the threads that are blocked on the specified
  condition variable cond (if any threads are blocked on cond).
  Verifies if the function was successful. */
void Cond_signal(pthread_cond_t *cond){
  if (pthread_cond_signal(cond) != 0)
    fprintf(stderr,"Error on pthread_cond_signal: %s\n",strerror(errno));
}

/* unblocks all threads currently blocked on the specified
condition variable cond. Verifies if the function was successful. */
void Cond_broadcast(pthread_cond_t *cond){
  if (pthread_cond_broadcast(cond) != 0)
    fprintf(stderr,"Error on Pthread_cond_broadcast: %s\n",strerror(errno));
}

/* Opens the file whose name is the string pointed to by path and associates
   a stream with it. Verifies if the function was successful. */
FILE *Fopen(const char *path, const char *mode){
  FILE *fp;
  fp = fopen(path, mode);
  if (fp == NULL) {
    fprintf(stderr,"Error on fopen: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("Opened file %s.\n",path);
  return fp;
}

/* Flushes the stream pointed to by fp and closes that file descriptor.
   Verifies if the function was successful. */
void Fclose(FILE *fp){
  if (fclose(fp) != 0) {
    fprintf(stderr,"Error on fclose: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("Closed log file.\n");
}

/* Reads the first line of an iteration from the log file and stores the
   iteration number.
   returns 0 in case of error or no characters read. */
int readLine1(list_t *plist){
  char line[MAX_STRING];

  if(fgets(line, MAX_STRING, plist->fp) == NULL)
    return 0;
  if (sscanf(line,"iteracao %d",&plist->iter) < 1) {
    fprintf(stderr,"Error on sscanf: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  return 1;
}

/* Reads the second line of an iteration from the log file. */
void readLine2(list_t *plist){
  char line[MAX_STRING];
  int pid = 0, exTime = 0;

  fgets(line, MAX_STRING, plist->fp);
  if (sscanf(line,"pid: %d execution time: %d s\n",&pid, &exTime) < 2) {
    fprintf(stderr,"Error on sscanf: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Reads the third line of an iteration from the log file.
returns the total execution time value, read from the log file. */
int readLine3(list_t *plist){
  char line[MAX_STRING];
  int totalExTime = 0;
  fgets(line, MAX_STRING, plist->fp);
  if (sscanf(line,"total execution time: %d s",&totalExTime) < 1) {
    fprintf(stderr,"Error on sscanf: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  return totalExTime;
}

/* Reads the file and check if its corrupted. */
void readFile(list_t *plist){
  int aux = 0, loop = 1;
  char line[MAX_STRING];

  while (loop) {
    loop = readLine1(plist);
    if (loop) {
      readLine2(plist);
      aux = readLine3(plist);
      if (aux < plist->totalExTime) {
        printf("Error. Log file corrupted\n");
        exit(EXIT_FAILURE);
      }
      plist->totalExTime = aux;
      /* Reads the fourth line of the iteration and ignores it. */
      fgets(line, MAX_STRING, plist->fp);
    }
  }
}

void fWritePid(list_t *plist, int pid, int exTime, char *endTime){
  /* writes the 4 lines of a new iteration. */
  if (fprintf(plist->fp,
    "iteracao %d\npid: %d execution time: %d s\ntotal execution time: %d s\nEnd date: %s"
    , plist->iter, pid, exTime, plist->totalExTime, endTime) < 0) {
    fprintf(stderr,"Error on fprintf: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  plist->iter++; /* increments the iteration value. */

  /* Flushes fp. Prints these lines to the log file. */
  if (fflush(plist->fp)) {
    fprintf(stderr,"Error on fflush: %s\n",strerror(errno));
  }
}

/* Removes a link to a file. If it's the last link, frees the memory occupied by
   that file. Verifies if the function was successful. */
void Unlink(const char *path){
  if (unlink(path)) {
    fprintf(stderr,"Error on unlink: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Creates a FIFO file with the specified pathname, and with the permissions
   specified in mode. Verifies if the function was successful. */
void Mkfifo(const char *pathname, mode_t mode){
  if (mkfifo(pathname, mode)) {
    fprintf(stderr,"Error on mkfifo: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Creates a new open fd and an entry in the system-wide table of open files.
   Verifies if the function was successful. */
int Open(const char *pathname, int flags, mode_t mode){
  int fd = -1;
  fd = open(pathname, flags, mode);
  if (fd == -1) {
    fprintf(stderr,"Error on open: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  return fd;
}

/* Closes a file descriptor. Verifies if the function was successful. */
void Close(int fd){
  if (close(fd)) {
    fprintf(stderr,"Error on close: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Copies the file descriptor to the lowest-numbered unused descriptor. Returns
   the new descriptor. Verifies if the function was successful. */
int Dup(int oldfd){
  int newfd = -1;
  newfd = dup(oldfd);
  if (newfd == -1) {
    fprintf(stderr,"Error on dup: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  return newfd;
}

/* Attempts to read up to count bytes from file descriptor fd into the buffer
starting at buf.
void Read() */

/* Sends any signal to any process group or process.
pid > 0 - sig is sent to that process.
pid = 0 - sig is sent to every process in the process group of the calling process.
pid = -1 - sig is broadcasted to all processes the calling process has permissions to signal.
pid < -1 - sig is sent to every process in the process group whose ID is -pid. */
void Kill(pid_t pid, int sig){
  if (kill(pid,sig)) {
    fprintf(stderr,"Error on kill: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Destroys previously created structures, and closes opened files. */
void cleanUp(list_t *plist, pthread_mutex_t *mutex, pthread_cond_t *CondMAX, pthread_cond_t *CondChild){
  lst_destroy(plist);
  destroyMutex(mutex);
  Cond_destroy(CondMAX);
  Cond_destroy(CondChild);
  Fclose(plist->fp);
}
