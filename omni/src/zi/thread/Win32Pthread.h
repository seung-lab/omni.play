/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_WIN32_PTHREAD_H_
#define _ZI_THREADS_WIN32_PTHREAD_H_

#include "Win32.h"

#ifdef ZI_WINDOWS
# include <windows.h>

#include <inttypes.h>

//
// MUTEX
//

typedef HANDLE pthread_mutex_t;
typedef void   pthread_mutexattr_t;

int pthread_mutex_init     (pthread_mutex_t *, const pthread_mutexattr_t *);
int pthread_mutex_destroy  (pthread_mutex_t *);
int pthread_mutex_lock     (pthread_mutex_t *);
int pthread_mutex_unlock   (pthread_mutex_t *);
int pthread_mutex_trylock  (pthread_mutex_t *);

//
// CONDITIONS
//

typedef struct {
  CRITICAL_SECTION noWaitersLock_;
  HANDLE           semaphore_;
  HANDLE           waitersDone_;
  int64_t          broadcast_;
  int64_t          noWaiters_;
} pthread_cond_t;

typedef void   pthread_condattr_t;

int pthread_cond_init      (pthread_cond_t *, const pthread_condattr_t *);
int pthread_cond_destroy   (pthread_cond_t *);
int pthread_cond_wait      (pthread_cond_t *, pthread_mutex_t *);
int pthread_cond_signal    (pthread_cond_t *);
int pthread_cond_broadcast (pthread_cond_t *);

//
// RWLOCK
//

typedef struct {
  CRITICAL_SECTION exclusiveLock_;
  int64_t noReaders_;
  int64_t noReadersDone_;
  pthread_mutex_t noReadersDoneLock_;
  int64_t noWriters_;
  pthread_cond_t cond_;
}  pthread_rwlock_t;

typedef void   pthread_rwlockattr_t;

int pthread_rwlock_init      (pthread_rwlock_t *, const pthread_rwlockattr_t*);
int pthread_rwlock_destroy   (pthread_rwlock_t *);
int pthread_rwlock_rdlock    (pthread_rwlock_t *);
int pthread_rwlock_tryrdlock (pthread_rwlock_t *);
int pthread_rwlock_wrlock    (pthread_rwlock_t *);
int pthread_rwlock_trywrlock (pthread_rwlock_t *);
int pthread_rwlock_unlock    (pthread_rwlock_t *);

//
// THREADS
//

struct sched_param {
  int sched_priority;
} ;

typedef struct {
  int    detached_;
  size_t stackSize_;
  int    schedPriority_;
} pthread_attr_t;

typedef struct {
  HANDLE handle_;
  DWORD  threadId_;
} pthread_t;

enum {
  PTHREAD_CREATE_JOINABLE = 0,
  PTHREAD_CREATE_DETACHED = 1
};

enum {
  SCHED_OTHER = 0,
  SCHED_FIFO,
  SCHED_RR
};

int pthread_attr_init           (pthread_attr_t *);
int pthread_attr_setdetachstate (pthread_attr_t *, int);
int pthread_attr_setstacksize   (pthread_attr_t *, size_t);
int pthread_attr_setschedpolicy (pthread_attr_t *, int);
int sched_get_priority_min      (int);
int sched_get_priority_max      (int);
int pthread_attr_setschedparam  (pthread_attr_t *, sched_param *);

int pthread_create (pthread_t *, const pthread_attr_t *, void *(*start) (void *), void *);
int pthread_join   (pthread_t, void*);

# endif
#endif
