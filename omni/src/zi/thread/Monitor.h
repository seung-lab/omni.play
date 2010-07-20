/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_MONITOR_H_
#define _ZI_THREADS_MONITOR_H_

#include "Win32.h"

#ifdef ZI_WINDOWS
# include "Win32Pthread.h"
# pragma once
#else
# include <pthread.h>
# include "Util.h"
#endif

namespace zi { namespace Threads {

class Monitor {

public:

  Monitor();
  virtual ~Monitor();

  virtual void lock()   const;
  virtual void unlock() const;
  virtual void wait(int64_t timeout=0LL) const;
  virtual void notify()    const;
  virtual void notifyAll() const;

protected:
  void cleanup();
  mutable bool             mutexInitialized_;
  mutable bool             condInitialized_;
  mutable pthread_mutex_t  pthread_mutex_;
  mutable pthread_cond_t   pthread_cond_;
};

class Synchronized {
public:
  Synchronized(const Monitor& value) : monitor_(value) { monitor_.lock(); }
  ~Synchronized() { monitor_.unlock(); }
private:
  const Monitor& monitor_;
};

} }

#endif
