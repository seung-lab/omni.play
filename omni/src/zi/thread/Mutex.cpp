/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include "Mutex.h"

#include <assert.h>

namespace zi { namespace Threads {

Mutex::Mutex() : initialized_(false) {
  int r = pthread_mutex_init(&pthread_mutex_, NULL);
  assert(r == 0);
  initialized_ = true;
}

Mutex::~Mutex() {
  if (initialized_) {
    initialized_ = false;
    int r = pthread_mutex_destroy(&pthread_mutex_);
    assert(r == 0);
  }
}

void Mutex::lock() const {
  int r = pthread_mutex_lock(&pthread_mutex_);
  assert(r == 0);
}

bool Mutex::trylock() const {
  return (0 == pthread_mutex_trylock(&pthread_mutex_));
}

void Mutex::unlock() const {
  int r = pthread_mutex_unlock(&pthread_mutex_);
}

RWMutex::RWMutex() : initialized_(false) {
  int r = pthread_rwlock_init(&rw_lock_, NULL);
  assert(r == 0);
  initialized_ = true;
}

RWMutex::~RWMutex() {
  if (initialized_) {
    initialized_ = false;
    int r = pthread_rwlock_destroy(&rw_lock_);
    assert(r == 0);
  }
}

void RWMutex::acquireRead() const {
  int r = pthread_rwlock_rdlock(&rw_lock_);
  assert(r == 0);
}

void RWMutex::acquireWrite() const {
  int r = pthread_rwlock_wrlock(&rw_lock_);
  assert(r == 0);
}

bool RWMutex::attemptRead() const {
  return (0 == pthread_rwlock_tryrdlock(&rw_lock_));
}

bool RWMutex::attemptWrite() const {
  return (0 == pthread_rwlock_trywrlock(&rw_lock_));
}

void RWMutex::release() const {
  int r = pthread_rwlock_unlock(&rw_lock_);
  assert(r == 0);
}

} }
