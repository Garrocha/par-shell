#ifndef _LIST_H_
#define _LIST_H_

/* lst_iitem - each element of the list points to the next element */
/* retInt -> stores the int value returned by exit() */
typedef struct lst_iitem {
  int pid;
  int retInt;
  time_t starttime;
  time_t endtime;
  struct lst_iitem *next;
} lst_iitem_t;

/* list_t */
/* numChildren: Stores the number of sons currently running */
/* ISEXIT: flag that is 0 if EXIT_COMMAND is called. */
/* iter - holds the value of the next iteration to print.
   totalExTime - holds the last total execution time of par-shell. */
typedef struct {
  lst_iitem_t * first;
  FILE *fp;
  int numChildren;
  int ISEXIT;
  int iter;
  int totalExTime;
} list_t;

list_t* lst_new();
void lst_destroy(list_t *list);
void insert_new_process(list_t *list, int pid, time_t starttime);
void rm_process(list_t *list, int p);
void update_terminated_process(list_t *plist, int pid, time_t endtime, int retInt);
void lst_print(list_t *list);

#endif
