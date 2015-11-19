#ifndef _UTILS_H_
#define _UTILS_H_

#include "list.h"

void mutexInit(pthread_mutex_t *mutex);
void destroyMutex(pthread_mutex_t *mutex);
void mutexLock(pthread_mutex_t *mutex);
void mutexUnlock(pthread_mutex_t *mutex); /* (*var) ou (var) / &var ou var */

/* Conditional variable functions. */
void Cond_init(pthread_cond_t *cond, const pthread_condattr_t *restrict attr);
void Cond_destroy(pthread_cond_t *cond);
void Cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
void Cond_signal(pthread_cond_t *cond);
void Cond_broadcast(pthread_cond_t *cond);

FILE *Fopen(const char *path, const char *mode);
void Fclose(FILE *fp);
int readLine1(list_t *plist);
void readLine2(list_t *plist);
int readLine3(list_t *plist);
void readFile(list_t *plist);
void fWritePid(list_t *plist, int pid, int exTime, char *endTime);

#endif
