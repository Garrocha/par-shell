#ifndef _SEMAPH_H_
#define _SEMAPH_H_

void Sem_init(sem_t *sem, int val);
void Sem_destroy(sem_t *sem);
void Sem_post(sem_t *sem);
void Sem_wait(sem_t *sem);


#endif
