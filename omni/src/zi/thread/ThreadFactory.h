/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_THREAD_FACTORY_H_
#define _ZI_THREADS_THREAD_FACTORY_H_

#include <boost/shared_ptr.hpp>

#include "Thread.h"

#ifdef ZI_WINDOWS
# include <windows.h>
# pragma once
#else
# include <pthread.h>
#endif

namespace zi { namespace Threads {

class ThreadFactory {

 public:

  enum SCHEDULE {
    OTHER  = SCHED_OTHER,
    FIFO   = SCHED_FIFO,
    RROBIN = SCHED_RR
  };

  enum PRIORITY {
    MINIMAL   = 0,
    VERYLOW   = 1,
    LOW       = 2,
    NORMAL    = 3,
    HIGH      = 4,
    VERYHIGH  = 5,
    MAXIMAL   = 6
  };

  ThreadFactory(SCHEDULE schedule = RROBIN,
                PRIORITY priority = NORMAL,
                int  stackSize = 1,
                bool detached = true);

  boost::shared_ptr<Thread>
  poopThread(boost::shared_ptr<Runnable> runnable) const;

  uint64_t myThreadId() const;
  static uint64_t ID();

  int      getStackSize() const;
  void     setStackSize(int value);
  bool     getDetached() const;
  void     setDetached(bool detached);
  void     setPriority(PRIORITY priority);
  PRIORITY getPriority() const;

 private:
  SCHEDULE schedule_;
  PRIORITY priority_;
  int      stackSize_;
  bool     detached_;
  int      minPriority_;
  int      maxPriority_;

};

} }

#endif
