#include "Win32Pthread.h"

#ifdef ZI_WINDOWS
# include <windows.h>

#include <inttypes.h>

//
// MUTEX
//

int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *)
{
  *m = CreateMutex(NULL, FALSE, NULL);
  return (*m == NULL);
}

int pthread_mutex_destroy(pthread_mutex_t *m)
{
  return CloseHandle(*m);
}

int pthread_mutex_lock(pthread_mutex_t *m)
{
  DWORD r = WaitForSingleObject(*m, INFINITE);
  return (r != WAIT_OBJECT_0);
}

int pthread_mutex_unlock(pthread_mutex_t *m)
{
  BOOL r = ReleaseMutex(*m);
  return (r != TRUE);
}

int pthread_mutex_trylock(pthread_mutex_t *m)
{
  DWORD r = WaitForSingleObject(*m, 0);
  return (r != WAIT_OBJECT_0);
}

//
// CONDITIONS
//

int pthread_cond_init(pthread_cond_t *cv,
                      const pthread_condattr_t *)
{
  cv->noWaiters_ = 0;
  cv->broadcast_ = 0;
  cv->semaphore_ = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);

  if (cv->semaphore_ == NULL)
    return 1;

  cv->waitersDone_ = CreateEvent(NULL, FALSE, FALSE, NULL);

  if (cv->waitersDone_ == NULL) {
    int ignore = CloseHandle(cv->semaphore_);
    return 1;
  }

  InitializeCriticalSection(&cv->noWaitersLock_);
  return 0;
}

int pthread_cond_destroy(pthread_cond_t *cv)

{
  DeleteCriticalSection(&cv->noWaitersLock_);
  BOOL ok = CloseHandle(cv->semaphore_) & CloseHandle(cv->waitersDone_);
  return (ok != TRUE);
}

int pthread_cond_wait(pthread_cond_t *cv,
                      pthread_mutex_t *mutex)
{
  EnterCriticalSection(&cv->noWaitersLock_);
  cv->noWaiters_++;
  LeaveCriticalSection(&cv->noWaitersLock_);

  DWORD wait = SignalObjectAndWait(*mutex, cv->semaphore_, INFINITE, FALSE);

  if (wait != WAIT_OBJECT_0)
    return 1;

  EnterCriticalSection(&cv->noWaitersLock_);
  cv->noWaiters_--;
  int last_waiter = cv->broadcast_ && cv->noWaiters_ == 0;
  LeaveCriticalSection(&cv->noWaitersLock_);

  if (last_waiter)
    wait = SignalObjectAndWait(cv->waitersDone_, *mutex, INFINITE, FALSE);
  else
    wait = WaitForSingleObject(*mutex, INFINITE);

  return (wait != WAIT_OBJECT_0);
}

int pthread_cond_signal(pthread_cond_t *cv)
{
  EnterCriticalSection(&cv->noWaitersLock_);
  int have_waiters = cv->noWaiters_ > 0;
  LeaveCriticalSection(&cv->noWaitersLock_);

  if (have_waiters)
    return (ReleaseSemaphore(cv->semaphore_, 1, 0) == 0);

  return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cv)
{
  EnterCriticalSection(&cv->noWaitersLock_);
  int have_waiters = 0;

  if (cv->noWaiters_ > 0) {
    cv->broadcast_ = 1;
    have_waiters   = 1;
  }

  if (have_waiters) {
    if (ReleaseSemaphore(cv->semaphore_, cv->noWaiters_, 0) == 0) {
      return 1;
    }
    LeaveCriticalSection(&cv->noWaitersLock_);
    if (WaitForSingleObject(cv->waitersDone_, INFINITE) != WAIT_OBJECT_0) {
      return 1;
    }
    cv->broadcast_ = 0;
  } else {
    LeaveCriticalSection(&cv->noWaitersLock_);
  }
  return 0;
}

//
// RWLOCK
//

int pthread_rwlock_init (pthread_rwlock_t *rwl, const pthread_rwlockattr_t*)
{
  rwl->noWriters_     = 0;
  rwl->noReadersDone_ = 0;
  rwl->noReaders_     = 0;
  InitializeCriticalSection(&rwl->exclusiveLock_);
  int r = pthread_mutex_init(&rwl->noReadersDoneLock_, NULL);
  if (r != 0)
    return r;
  r = pthread_cond_init(&rwl->cond_, NULL);
  if (r != 0) {
    pthread_mutex_destroy(&rwl->noReadersDoneLock_);
    return r;
  }
  return 0;
}

int pthread_rwlock_destroy (pthread_rwlock_t *rwl)
{
  int rm = pthread_mutex_destroy(&rwl->noReadersDoneLock_);
  int rc = pthread_cond_destroy(&rwl->cond_);
  DeleteCriticalSection(&rwl->exclusiveLock_);
  return (rm | rc);
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwl)
{
  EnterCriticalSection(&rwl->exclusiveLock_);
  rwl->noReaders_++;
  LeaveCriticalSection(&rwl->exclusiveLock_);
  return 0;
}

int pthread_rwlock_tryrdlock (pthread_rwlock_t *rwl)
{
  if (TryEnterCriticalSection(&rwl->exclusiveLock_)) {
    rwl->noReaders_++;
    LeaveCriticalSection(&rwl->exclusiveLock_);
    return 0;
  }
  return 1;
}

int pthread_rwlock_wrlock (pthread_rwlock_t *rwl)
{
  EnterCriticalSection(&rwl->exclusiveLock_);
  int r = pthread_mutex_lock(&rwl->noReadersDoneLock_);
  if (r != 0)
    return r;

  if (rwl->noWriters_ == 0) {

    if (rwl->noReadersDone_ > 0) {
      rwl->noReaders_ -= rwl->noReadersDone_;
      rwl->noReadersDone_ = 0;
    }

    if (rwl->noReaders_ > 0) {
      rwl->noReadersDone_ = -rwl->noReaders_;
      int r;
      do {
        r = pthread_cond_wait(&rwl->cond_,
                          &rwl->noReadersDoneLock_);
      } while ((r == 0) & (rwl->noReadersDone_ < 0));

      if (r != 0) {
        int ignore = pthread_mutex_unlock(&rwl->noReadersDoneLock_);
        LeaveCriticalSection(&rwl->exclusiveLock_);
        return r;
      }
      rwl->noReadersDone_ = 0;
    }
  }

  rwl->noWriters_++;
  return 0;
}

int pthread_rwlock_trywrlock (pthread_rwlock_t *rwl)
{
  if (TryEnterCriticalSection(&rwl->exclusiveLock_)) {

    int r = pthread_mutex_trylock(&rwl->noReadersDoneLock_);
    if (r != 0) {
      LeaveCriticalSection(&rwl->exclusiveLock_);
      return r;
    }

    if (rwl->noWriters_ == 0) {

      if (rwl->noReadersDone_ > 0) {
        rwl->noReaders_ -= rwl->noReadersDone_;
        rwl->noReadersDone_ = 0;
      }

      if (rwl->noReaders_ > 0) {
        int r = pthread_mutex_unlock(&rwl->noReadersDoneLock_);
        LeaveCriticalSection(&rwl->exclusiveLock_);
        if (r != 0)
          return r;
      } else {
        rwl->noWriters_ = 1;
      }
    }
    return 0;
  }
  return 1;
}

int pthread_rwlock_unlock (pthread_rwlock_t *rwl)
{
  if (rwl->noWriters_ == 0) {

    int r = pthread_mutex_lock(&rwl->noReadersDoneLock_);
    if (r != 0)
      return r;

    if (++rwl->noReaders_ == 0) {
      int r = pthread_cond_signal(&rwl->cond_);
      if (r != 0) {
        int ignore = pthread_mutex_unlock(&rwl->noReadersDoneLock_);
        return r;
      }
    }
    r = pthread_mutex_unlock(&rwl->noReadersDoneLock_);
    if (r != 0)
      return r;

  } else {
    rwl->noWriters_--;
    int r = pthread_mutex_unlock(&rwl->noReadersDoneLock_);
    if (r != 0)
      return r;
    LeaveCriticalSection(&rwl->exclusiveLock_);
  }
  return 0;
}

//
// THREADS
//

int pthread_attr_init (pthread_attr_t *attr)
{
  attr->detached_ = PTHREAD_CREATE_DETACHED;
  return 0;
}

int pthread_attr_setdetachstate (pthread_attr_t * attr, int detachstate)
{
  attr->detached_ = detachstate;
  return 0;
}

int pthread_attr_setstacksize   (pthread_attr_t * attr, size_t size)
{
  attr->stackSize_ = size;
  return 0;
}

int pthread_attr_setschedpolicy (pthread_attr_t * attr, int policy)
{
  return 0;
}

int sched_get_priority_min(int) {
  return THREAD_PRIORITY_IDLE;
}

int sched_get_priority_max(int) {
  return THREAD_PRIORITY_HIGHEST;
}

int pthread_attr_setschedparam (pthread_attr_t *attr, sched_param *sparam) {
  attr->schedPriority_ = sparam->sched_priority;
  return 0;
}

int pthread_create (pthread_t * tid,
                    const pthread_attr_t * attr,
                    void *(*start) (void *), void *arg)
{
  DWORD  threadID;
  tid->handle_ = CreateThread(NULL, attr->stackSize_,
                              (LPTHREAD_START_ROUTINE) start,
                              (LPVOID)arg, CREATE_SUSPENDED, &threadID);

  tid->threadId_ = threadID;

  if (!SetThreadPriority(tid->handle_, attr->schedPriority_)) {
    int ignore = CloseHandle(tid->handle_);
    return 1;
  }

  if (ResumeThread(tid->handle_) == -1) {
    int ignore = CloseHandle(tid->handle_);
    return 1;
  }
  return 0;
}

int pthread_join (pthread_t t, void*)
{
  int r = WaitForSingleObject(t.handle_, INFINITE);
  if (r != WAIT_OBJECT_0) {
    return 1;
  }
  return CloseHandle(t.handle_);
}

#endif
