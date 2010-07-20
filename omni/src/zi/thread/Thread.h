/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_THREAD_H_
#define _ZI_THREADS_THREAD_H_

#include "Win32.h"

#ifdef ZI_WINDOWS
# include "Win32Pthread.h"
# pragma once
#else
# include <pthread.h>
#endif

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace zi { namespace Threads {

class Thread;

class Runnable {

public:
  virtual ~Runnable() {};
  virtual void run() = 0;
  virtual boost::shared_ptr<Thread> thread() { return thread_.lock(); }
  virtual void thread(boost::shared_ptr<Thread> value) {
    thread_ = boost::weak_ptr<Thread>(value);
  }

private:
  boost::weak_ptr<Thread> thread_;
};

class Thread {

public:

  enum STATE {
    WAITING,
    RUNNING,
    DONE
  };

  static void* MAIN(void* arg);

  Thread(int schedule,
         int priority,
         int      stackSize,
         bool     isDetached,
         boost::shared_ptr<Runnable> runnable);

  virtual ~Thread();

  virtual void start();
  virtual void join();
  virtual uint64_t getId();

  virtual boost::shared_ptr<Runnable> runnable() const;
  void setWeakRef(boost::shared_ptr<Thread> self);

protected:
  boost::shared_ptr<Runnable> runnable_;
  pthread_t pthread_;
  STATE     state_;
  int       schedule_;
  int       priority_;
  int       stackSize_;
  boost::weak_ptr<Thread> self_;
  bool      isDetached_;
};

} }

#endif
