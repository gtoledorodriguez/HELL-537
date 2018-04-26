#ifndef _THREAD_LOCK_H
#define _THREAD_LOCK_H

typedef struct lock_t {
  uint ticket;
  uint turn;
}lock_t;

#endif
