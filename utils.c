#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>  /* kill */
#include <sys/stat.h>   /* mkfifo */
#include <fcntl.h>      /* open */
#include <signal.h>     /* signal, kill */

#include "utils.h"

#define MAX_STRING 512

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

/**********************************************************
Kill: ends any signal to any process group or process.
  pid > 0 - sig is sent to that process.
  pid = 0 - sig is sent to every process in the process group of the calling process.
  pid = -1 - sig is broadcasted to all processes the calling process has permissions to signal.
  pid < -1 - sig is sent to every process in the process group whose ID is -pid.
*********************************************************/
void Kill(pid_t pid, int sig){
  if (kill(pid,sig)) {
    fprintf(stderr,"Error on kill: %s\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/**********************************************************
Write: Sends a msg using the pipe of fd
*********************************************************/
void Write(int fdPipe, char *msg){
  long nbytes, nleft, nwritten;
  char *ptr;

  /* set for the current msg to write. */
  nbytes = strlen(msg);
  nleft = nbytes;
  ptr = msg;
  nwritten = 0;

  /* Writes until all bits from the msg are written */
  while (nleft > 0) {
    nwritten = write(fdPipe, ptr, nleft);
    if (nwritten <= 0){
      /* errno should only be used if it was previously assigned. */
      if (errno != EPIPE) {
        printf("Error on writting: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
      }
    }
    if (nwritten == -1){
      if (errno == EPIPE){
        printf("Write -> Other side closed it's connection to pipe.\n");
        exit(EXIT_FAILURE);
      }
    }
    nleft -= nwritten;
    ptr += nwritten;
    nwritten = 0;
  }
  printf("FD %d Sent msg:%s", fdPipe, msg);

}

/**********************************************************
Read: Reads from a fd and stores the msg on the buffer
*********************************************************/
void Read(int fdPipe){
  long msglen = 0;
  char newmsg[MAX_STRING+1];
  char aux[MAX_STRING+1];
  memset(newmsg,(int) '\0', MAX_STRING);
  memset(aux,(int) '\0', MAX_STRING);

  while(strchr(newmsg, (int) '\n') == NULL){
    msglen = read(fdPipe, aux, MAX_STRING);
    if (msglen < 0) {
      fprintf(stderr, "Error on reading: %s\n",strerror(errno));
      exit(EXIT_FAILURE);
    }
    strcat(newmsg, aux);
  }
  printf("recebi %s", newmsg);
}
