#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "list.h"
#include "utils.h"

/**********************************************************
 lst_new: allocates memory for a list_t list and initializes it.
 *********************************************************/
list_t* lst_new(){
  extern int errno;
  list_t *list;
  list = (list_t*) malloc(sizeof(list_t));
  if (list == NULL){
    fprintf(stderr,"Error on malloc: %s\n",strerror(errno));
    exit(-2);
  }
  list->first = NULL;
  list->fp = NULL;
  list->numChildren = 0;
  list->ISEXIT = 1;
  list->iter = 0;
  list->totalExTime = 0;
  return list;
}

/**********************************************************
 lst_destroy: frees memory of 'list' and all its items.
 *********************************************************/
void lst_destroy(list_t *list){
  struct lst_iitem *item, *nextitem;

  item = list->first;
  while (item != NULL){
    nextitem = item->next;
    free(item);
    item = nextitem;
  }
  free(list);
}

/**********************************************************
 insert_new_process: inserts a new item with process id and its start time
 at the end of list 'list' and increments numChildren.
 *********************************************************/
void insert_new_process(list_t *list, int pid, time_t starttime){
  lst_iitem_t *item;
  item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
  if (item == NULL){
    fprintf(stderr,"Error on malloc: %s\n",strerror(errno));
    exit(-2);
  }
  item->pid = pid;
  item->starttime = starttime;
  item->endtime = 0;
  item->next = list->first;
  list->first = item;
  list->numChildren++;
}

/**********************************************************
 rm_process: Removes process with pid 'pid' from the list and decrements numChildren.
 *********************************************************/
void rm_process(list_t *list, int pid){
  lst_iitem_t *aux, *prev;
  aux = list->first;
  prev = NULL;

  while (aux != NULL) {
    if (aux->pid == pid) {
      /* caso da primeira posicao. */
      if (aux == list->first) {
        list->first = aux->next;
      }else
        prev->next = aux->next;

      free(aux);
      list->numChildren--;
    }else{
      prev = aux;
      aux = aux->next;
    }
  }

}

/**********************************************************
 update_terminated_process: updates endtime of element with pid 'pid'
  and its return value. Decrements numChildren since the process isn't running anymore.
  Prints to the log file the terminated process pid, execution and total time.
 *********************************************************/
void update_terminated_process(list_t *plist, int pid, time_t endtime, int retInt){

  lst_iitem_t * aux;
  aux = plist->first;
  while(aux != NULL){
    if(aux->pid == pid){
      aux->endtime = endtime;
      aux->retInt = retInt;
      plist->numChildren--;
      plist->totalExTime += (int)(aux->endtime - aux->starttime);
      fWritePid(plist, aux->pid, (int)(aux->endtime - aux->starttime),ctime(&(aux->endtime)));
      return;
    }
    aux = aux->next;
  }
  /* Only runs in case of error. */
  printf("Error on update_terminated_process(): pid %d not in list.\n", pid);
}

/**********************************************************
 lst_print: print the content of list 'list' to stdout.
 *********************************************************/
void lst_print(list_t *list){
  lst_iitem_t *item;
  printf("Process list with start, end time and return value:\n");
  item = list->first;
  while (item != NULL){
    printf("Process: %d\n\tStart time: %s", item->pid, ctime(&(item->starttime)));
    printf("\tExecution time: %d s\n", (int)(item->endtime - item->starttime));
    printf("\tReturn value: %d\n",item->retInt);
    item = item->next;
  }
  printf("-- end of list.\n");
}
