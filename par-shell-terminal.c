#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> /* open */
#include <sys/stat.h>  /* open */
#include <fcntl.h>     /* open */

#include "utils.h"

//#define IN_PIPE " " /*name of the amed pipe created by par-shell.*/
#define BUFFER_SIZE 512
#define PPERMS 07777    /* Permissions for mkfifo. */
#define CMD_STATS "stats\n"
#define CMD_EXIT "exit\n"
#define CMD_EXITG "exit-global\n"

/**********************************************************
 printcmds: Prints all the commands of the program
 *********************************************************/
 void printcmds(){

  printf("------------par-shell-terminal-----------\n");
  printf("----------- List of commands: -----------\n");
  printf("--- <pathname> <arg1> ... <arg5>      ---\n");
  printf("--- stats                             ---\n");
  printf("--- exit                              ---\n");
  printf("--- exit-global                       ---\n");
  printf("-----------------------------------------\n");
  return;
}


int main(int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Invalid arguments\n");
    exit(EXIT_FAILURE);
  }
  int namedPipeFd = -1;
  int loop = 1;
  char namedPipe[BUFFER_SIZE];
  char msg[BUFFER_SIZE+1];      /* Message to send through named pipe. */

  /* The first argument is the name of the named pipe. */
  strcpy(namedPipe,argv[1]);
  printf("vou abrir pipe\n");
  namedPipeFd = Open(namedPipe, O_WRONLY, S_IWUSR);   /* ESTA CERTO?? */
  printf("abri pipe!\n");

  printcmds();
  while (loop){
    memset(msg,(int) '\0', BUFFER_SIZE+1);
    fgets(msg, BUFFER_SIZE, stdin);
    printf("escreveu %s", msg);
    if (strcmp(msg, CMD_EXIT) == 0){
      loop = 0;
    } else if(strcmp(msg, CMD_EXITG) == 0) {
      Write(namedPipeFd, CMD_EXIT);
    }else Write(namedPipeFd, msg);
  }

  Close(namedPipeFd);
  exit(EXIT_SUCCESS);
}
