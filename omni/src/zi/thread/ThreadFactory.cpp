/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <assert.h>

#include <boost/shared_ptr.hpp>

#include "ThreadFactory.h"

namespace zi { namespace Threads {

using boost::shared_ptr;

ThreadFactory::ThreadFactory(ThreadFactory::SCHEDULE schedule,
                             ThreadFactory::PRIORITY priority,
                             int  stackSize,
                             bool detached) :
  schedule_(schedule),
  priority_(priority),
  stackSize_(stackSize),
  detached_(detached)
{
#ifdef ZI_WINDOWS
  minPriority_ = 0;
  maxPriority_ = 1;
#else
  minPriority_ = sched_get_priority_min(schedule);
  maxPriority_ = sched_get_priority_max(schedule);
#endif
}

shared_ptr<Thread>
ThreadFactory::poopThread(shared_ptr<Runnable> runnable) const {

  int pThreadPriority =
    minPriority_ + (priority_ * (maxPriority_ - minPriority_))
    / ThreadFactory::MAXIMAL;

  shared_ptr<Thread> newOne =
    shared_ptr<Thread>
    (new Thread(schedule_,
                pThreadPriority,
                stackSize_, detached_, runnable));

  newOne->setWeakRef(newOne);
  runnable->thread(newOne);
  return newOne;
}

int ThreadFactory::getStackSize() const {
  return stackSize_;
}

void ThreadFactory::setStackSize(int value) {
  stackSize_ = value;
}

void ThreadFactory::setPriority(PRIORITY value) {
  priority_ = value;
}

bool ThreadFactory::getDetached() const {
  return detached_;
}

void ThreadFactory::setDetached(bool value) {
  detached_ = value;
}

uint64_t ThreadFactory::myThreadId() const {
#ifdef ZI_WINDOWS
  return (uint64_t)GetCurrentThreadId();
#else
  return (uint64_t)pthread_self();
#endif
}

uint64_t ThreadFactory::ID() {
#ifdef ZI_WINDOWS
  return (uint64_t)GetCurrentThreadId();
#else
  return (uint64_t)pthread_self();
#endif
}


ThreadFactory::PRIORITY ThreadFactory::getPriority() const {
  return priority_;
}


} }
