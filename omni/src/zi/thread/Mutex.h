/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_MUTEX_H_
#define _ZI_THREADS_MUTEX_H_

#include "Win32.h"

#ifdef ZI_WINDOWS
# include "Win32Pthread.h"
# pragma once
#else
# include <pthread.h>
#endif

namespace zi { namespace Threads {

class Mutex {

public:
  Mutex();
  virtual ~Mutex();
  virtual void lock()    const;
  virtual bool trylock() const;
  virtual void unlock()  const;
private:
  mutable pthread_mutex_t pthread_mutex_;
  mutable bool initialized_;
};

class RWMutex {
public:
  RWMutex();
  virtual ~RWMutex();

  virtual void acquireRead()  const;
  virtual void acquireWrite() const;
  virtual bool attemptRead()  const;
  virtual bool attemptWrite() const;
  virtual void release()      const;

private:
  mutable pthread_rwlock_t rw_lock_;
  mutable bool initialized_;
};

class Guard {
public:
  Guard(const Mutex& value) : mutex_(value) { mutex_.lock(); }
  ~Guard() { mutex_.unlock(); }
private:
  const Mutex& mutex_;
};


class RWGuard {
public:
  RWGuard(const RWMutex& value, bool write = 0)
    : rwMutex_(value), write_(write)
  {
    if (write) {
      rwMutex_.acquireWrite();
    } else {
      rwMutex_.acquireRead();
    }
  }
  ~RWGuard() { rwMutex_.release(); }

  void toWrite() {
    if (!write_) {
      rwMutex_.release();
      rwMutex_.acquireWrite();
      write_ = true;
    }
  }

  void toRead() {
    if (write_) {
      rwMutex_.release();
      rwMutex_.acquireRead();
      write_ = false;
    }
  }

private:
  const RWMutex& rwMutex_;
  bool  write_;
};

} }

#endif
