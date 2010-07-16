/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <assert.h>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "Exception.h"
#include "Thread.h"

namespace zi { namespace Threads {

using boost::shared_ptr;
using boost::weak_ptr;

Thread::Thread(int  schedule,
               int  priority,
               int  stackSize,
               bool isDetached,
               shared_ptr<Runnable> runnable) :
  state_(WAITING),
  schedule_(schedule),
  priority_(priority),
  stackSize_(stackSize),
  isDetached_(isDetached)
{
  runnable_ = runnable;
}

Thread::~Thread() {
  if(!isDetached_) {
    try {
      join();
    } catch(...) {
      assert(0);
    }
  }
}

void Thread::start() {

  if (state_ != WAITING)
    return;

  pthread_attr_t thread_attr;

  if (pthread_attr_init(&thread_attr)) {
    throw ThreadException("pthread_attr_init");
  }

  if (pthread_attr_setdetachstate(&thread_attr,
                                  isDetached_ ? PTHREAD_CREATE_DETACHED
                                  : PTHREAD_CREATE_JOINABLE)) {
    throw ThreadException("pthread_attr_setdetachstate");
  }

  if (pthread_attr_setstacksize(&thread_attr, stackSize_ << 20)) {
    throw ThreadException("pthread_attr_setstacksize");
  }

  if (pthread_attr_setschedpolicy(&thread_attr, schedule_)) {
    throw ThreadException("pthread_attr_setschedschedule");
  }

  struct sched_param sched_param;
  sched_param.sched_priority = priority_;

  if (pthread_attr_setschedparam(&thread_attr, &sched_param)) {
    throw ThreadException("pthread_attr_setschedparam");
  }

  shared_ptr<Thread>* self = new shared_ptr<Thread>();
  *self = self_.lock();

  state_ = RUNNING;

  if (pthread_create(&pthread_, &thread_attr, MAIN, (void*)self)) {
    throw ThreadException("pthread_create");
  }
}

void Thread::join() {
  if (!isDetached_ && state_ != WAITING) {
    void* ignore;
    isDetached_ = pthread_join(pthread_, &ignore) == 0;
  }
}

uint64_t Thread::getId() {
#ifdef ZI_WINDOWS
  return (uint64_t)pthread_.threadId_;
#else
  return (uint64_t)pthread_;
#endif
}

shared_ptr<Runnable> Thread::runnable() const {
  return runnable_;
}

void* Thread::MAIN(void* arg) {
  shared_ptr<Thread> thread = *(shared_ptr<Thread>*)arg;
  delete reinterpret_cast<shared_ptr<Thread>*>(arg);

  if (thread == NULL) return (void*)0;

  if (thread->state_ != RUNNING) return (void*)0;

  thread->runnable()->run();
  thread->state_ = DONE;
  return (void*)0;
}

void Thread::setWeakRef(shared_ptr<Thread> self) {
  if (self.get() != this)
    assert(0);
  self_ = weak_ptr<Thread>(self);
}

} }


