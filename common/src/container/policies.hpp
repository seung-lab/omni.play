#pragma once
#include "precomp.h"

namespace om {
namespace data {

// Async Policies //////////////////////////////

class NoAsyncPolicy {
 protected:
  template <typename TRet>
  TRet asnyc_do(std::function<TRet()> f) {
    return f();
  }

  template <typename TRet, typename TArg, typename InputIterator>
  TRet asnyc_do(std::function<TRet(TArg)> f, InputIterator begin,
                InputIterator end) {
    for (; begin != end; ++begin) return f(*begin);
  }
};

// Locking Policies //////////////////////////////

template <class T>
struct lock_guard {
  lock_guard(const T& lockable) : lockable_(lockable) { lockable_.lock(); }
  ~lock_guard() { lockable_.unlock(); }

  const T& lockable_;
};

class NoLockingPolicy {
 protected:
  void lock() const {}
  void unlock() const {}
  template <typename T>
  friend class lock_guard;
};

class ZiLockingPolicy {
 protected:
  void lock() const { return mutex_.lock(); }
  void unlock() const { return mutex_.unlock(); }
  mutable zi::mutex mutex_;
  template <typename T>
  friend class lock_guard;
};

// Exception Policies //////////////////////////////

template <typename TKey, typename TValue>
class NoThrowPolicy {
 protected:
  void throw_invalid_index(TKey idx) const {}
};

template <typename TKey, typename TValue>
class ThrowPolicy {
 protected:
  void throw_invalid_index(TKey idx) const {
    throw ArgException("Invalid Index", idx);
  }
};

// Storage Policies ////////////////////////////////

template <typename T>
class VectorBackedStore {
 public:
  size_t size() const { return vec_.size(); }
  void resize(size_t n, const T& val) { vec_.resize(n, val); }
  void reserve(size_t n) { vec_.resreve(n); }
  T& doGet(size_t i) { return vec_[i]; }
  const T& doGet(size_t i) const { return vec_[i]; }

  void flush() {}

 private:
  std::vector<T> vec_;
};
}
}