#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#include "list.h"
#include "commandlinereader.h"
#include "utils.h"

#define MAXARG 7 /* CMD + 5 ARGS + NULL = 7 */
#define BUFFER_SIZE 100
#define MAXPAR 2    /* max number of child processes. */
#define THREADS 1   /* size of thread vector. */
#define EXIT_COMMAND "exit"
#define CLEAR_COMMAND "clear"
#define MORE_COMMAND "more"
#define FILE_NAME "log.txt" /* name of the file where data is stored. */
#define MODE_APLUS "a+"     /* mode of opening a file that writes at the EOF.*/

list_t *plist;
time_t setTime;
pthread_mutex_t mutex;
pthread_cond_t CondMAX; /* Condition variable for MAXPAR */
pthread_cond_t CondChild; /* Condition variable for number of running child processes. */
int valMAXPAR;            /* Stores the current MAXPAR value. */

/* Monitor thread. */
void *monitor(){
  int terminated_pid = 0, status = 0, loop = 1;

  while(loop){
    mutexLock(&mutex);
    /* wait condchild, remove espera ativa */
    while(plist->numChildren == 0 && plist->ISEXIT == 1){Cond_wait(&CondChild, &mutex);}
    mutexUnlock(&mutex);
      if (!plist->ISEXIT && plist->numChildren == 0)
        loop = 0;
      else{
        while(plist->numChildren > 0){
          /* Receives the exit signal from terminated child processes. */
          terminated_pid = wait(&status);
          if (terminated_pid < 0){
            fprintf(stderr, "Error on wait: %s\n", strerror(errno));
            mutexLock(&mutex);              /* Closes mutex. */
            plist->numChildren--;
            mutexUnlock(&mutex);            /* Opens mutex. */
          }
          else if (WIFEXITED(status)){
            /* the child terminated normally. */
            mutexLock(&mutex);              /* Closes mutex. */
            time(&setTime);
            update_terminated_process(plist, terminated_pid, setTime, WEXITSTATUS(status));
            Cond_signal(&CondMAX);                    /* Post */
            mutexUnlock(&mutex);            /* Opens mutex. */
          }else{
            /* child didn't terminated normally. */
            mutexLock(&mutex);              /* Closes mutex. */
            rm_process(plist,terminated_pid);
            Cond_signal(&CondMAX);                    /* Post */
            mutexUnlock(&mutex);            /* Opens mutex. */
          }

        }
      }   /* num > 0 */
    }
    pthread_exit(NULL);
  }

int main(int argc, char **argv){
  char *input[MAXARG];
  char buffer[BUFFER_SIZE];
  int cmd = 0, readVal = 0, pid = 0;
  valMAXPAR = MAXPAR;
  extern int errno;
  pthread_t tid[THREADS];
  plist = lst_new();
  plist->fp = Fopen(FILE_NAME,MODE_APLUS);

  readFile(plist);
  /* If the log file has previous entries, increments the last iter read
     so it has the correct value of the next iteration to print. */
  if (plist->iter != 0)
    plist->iter++;

  mutexInit(&mutex);

  if (pthread_create(&tid[0],0,monitor,NULL) != 0) {
    fprintf(stderr,"Error on pthread_create: %s\n",strerror(errno));
  }

  Cond_init(&CondChild,NULL);
  Cond_init(&CondMAX,NULL);

  printcmds(0);
  while (plist->ISEXIT){
    cmd = 0;

    readVal = readLineArguments(input,MAXARG,buffer,BUFFER_SIZE);
    if (readVal < 0){
      fprintf(stderr, "Error on readLineArguments: %s\n",strerror(errno));
      lst_destroy(plist);
      destroyMutex(&mutex);
      Cond_destroy(&CondMAX);
      Cond_destroy(&CondChild);
      Fclose(plist->fp);
      exit(EXIT_FAILURE);
    }

    if (readVal > 0){
      /* Clear cmd */
      if (strcmp(input[0],CLEAR_COMMAND) == 0){
        cmd = 1;
        system ("clear");
        printcmds(0);
      }

      /* more cmd */
      if (strcmp(input[0],MORE_COMMAND) == 0) {
        cmd = 1;
        mutexLock(&mutex);
        Cond_signal(&CondMAX);                    /* Post */
        valMAXPAR++;
        mutexUnlock(&mutex);
        printf("Incremented max parallel child processes.\n");
      }

      /* Exit cmd */
      if (strcmp(input[0],EXIT_COMMAND) == 0){
        cmd = 1;

        mutexLock(&mutex);              /* Closes mutex. */
        plist->ISEXIT = 0;                        /* program will exit. */
        /* Unlocks monitor thread in order for it to leave. */
        Cond_signal(&CondChild);
        mutexUnlock(&mutex);            /* Opens mutex. */

        /* Waits for the monitor thread to exit. */
        if (pthread_join(tid[0],NULL) != 0) {
          fprintf(stderr,"Error on pthread_join: %s\n",strerror(errno));
        }
      }

      /* Launches child process if no cmd was invoked */
      if (!cmd){
        printf("Inicializing process on file %s\n", input[0]);
        mutexLock(&mutex);              /* Closes mutex. */
        /* Wait MAXPAR - limits the number of max child processes. */
        while(plist->numChildren == valMAXPAR){Cond_wait(&CondMAX,&mutex);}
        mutexUnlock(&mutex);            /* Opens mutex. */
        pid = fork();
        if (pid < 0){
          fprintf(stderr, "Error on fork: %s\n",strerror(errno));
        }

        if (pid > 0){     /* Parent process */
          mutexLock(&mutex);              /* Closes mutex. */
          time(&setTime);
          insert_new_process(plist, pid, setTime);
          Cond_signal(&CondChild);
          mutexUnlock(&mutex);            /* Opens mutex. */
        }

        if (pid == 0){                              /* Child process */
        	execv(input[0], input);
          /* New program in charge, the code below runs in case an error ocurred with execv.*/
          fprintf(stderr,"Error on execv: %s\n",strerror(errno));
          /*lst_destroy(plist);
          destroyMutex(&mutex);
          Cond_destroy(&CondMAX);
          Cond_destroy(&CondChild);
          */
          exit(EXIT_FAILURE);
        }
      }

    }
  } /* end of loop */

  if (plist->first != NULL){
    lst_print(plist);
  }else
    printf("Nothing to print\n");
  lst_destroy(plist);
  destroyMutex(&mutex);
  Cond_destroy(&CondMAX);
  Cond_destroy(&CondChild);
  Fclose(plist->fp);
  printf("Exiting par-shell..\n");
  exit(EXIT_SUCCESS);
}
