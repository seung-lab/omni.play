/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <assert.h>
#include <errno.h>

#include <iostream>

#include "Exception.h"
#include "Monitor.h"

namespace zi { namespace Threads {

Monitor::Monitor() : mutexInitialized_(false), condInitialized_(false) {

  if (pthread_mutex_init(&pthread_mutex_, NULL) == 0) {
    mutexInitialized_ = true;
    condInitialized_  = (pthread_cond_init(&pthread_cond_, NULL) == 0);
  }
  if (!mutexInitialized_ || !condInitialized_) {
    cleanup();
    assert(0); // should fail!
  }
}

Monitor::~Monitor() {
  cleanup();
}

void Monitor::lock() const {
  int r = pthread_mutex_lock(&pthread_mutex_);
  assert(r == 0);
}

void Monitor::unlock() const {
  int r = pthread_mutex_unlock(&pthread_mutex_);
  assert(0 == r);
}

void Monitor::wait(int64_t timeout) const {
  assert(timeout >= 0LL);
  if (timeout == 0LL) {
    int r = pthread_cond_wait(&pthread_cond_, &pthread_mutex_);
    assert(0 == r);
  } else {
#ifdef ZI_WINDOWS
    int r = pthread_cond_wait(&pthread_cond_, &pthread_mutex_);
    assert(0 == r);
#else
    struct timespec abstime;
    int64_t now = Util::currentTime();
    Util::toTimespec(abstime, now + timeout);
    int result = pthread_cond_timedwait(&pthread_cond_,
                                        &pthread_mutex_,
                                        &abstime);
    if (result == ETIMEDOUT) {
      throw TimedOutException();
    }
#endif
  }
}

void Monitor::notify() const {
  int r = pthread_cond_signal(&pthread_cond_);
  assert(0 == r);
}

void Monitor::notifyAll() const {
  int r = pthread_cond_broadcast(&pthread_cond_);
  assert(0 == r);
}

void Monitor::cleanup() {
  if (mutexInitialized_) {
    mutexInitialized_ = false;
    int r = pthread_mutex_destroy(&pthread_mutex_);
    assert(0 == r);
  }

  if (condInitialized_) {
    condInitialized_ = false;
    int r = pthread_cond_destroy(&pthread_cond_);
    assert(0 == r);
  }
}

} }
