#pragma once

#include <vector>
#include "common/indexedIterator.hpp"
#include <zi/mutex.hpp>

namespace om {
namespace data {

// Async Policies //////////////////////////////

class NoAsyncPolicy {
 protected:
  template <typename TRet> TRet asnyc_do(std::function<TRet()> f) {
    return f();
  }

  template <typename TRet, typename TArg, typename InputIterator>
  TRet asnyc_do(std::function<TRet(TArg)> f, InputIterator begin,
                InputIterator end) {
    for (; begin != end; ++begin) return f(*begin);
  }
};

// Locking Policies //////////////////////////////

class NoLockingPolicy {
 protected:
  void lock() const {}
  void unlock() const {}
};

class ZiLockingPolicy {
 protected:
  void lock() const { return mutex_.lock(); }
  void unlock() const { return mutex_.unlock(); }
  mutable zi::mutex mutex_;
};

// Exception Policies //////////////////////////////

template <typename TKey, typename TValue> class NoThrowPolicy {
 protected:
  void throw_invalid_index(TKey idx) const {}
};

template <typename TKey, typename TValue> class ThrowPolicy {
 protected:
  void throw_invalid_index(TKey idx) const {
    throw ArgException("Invalid Index", idx);
  }
};

// Storage Policies ////////////////////////////////

template <typename T> class VectorBackedStore {
 public:
  size_t size() const { return vec_.size(); }
  void resize(size_t n, const T& val) { vec_.resize(n, val); }
  T& doGet(size_t i) { return vec_[i]; }
  const T& doGet(size_t i) const { return vec_[i]; }

  void flush() {}

 private:
  std::vector<T> vec_;
};

// Vector Type  /////////////////////////////////

template <typename T, typename StoragePolicy = VectorBackedStore<T>,
          typename AsyncPolicy = NoAsyncPolicy,
          typename LockingPolicy = NoLockingPolicy,
          typename ExceptionPolicy = ThrowPolicy<size_t, T>>
class vector : public LockingPolicy,
               public ExceptionPolicy,
               public AsyncPolicy {
 protected:
  typedef vector<T, StoragePolicy, AsyncPolicy, LockingPolicy, ExceptionPolicy>
      this_t;

 public:
  vector(StoragePolicy&& store = StoragePolicy()) : store_(store) {}

  virtual T& operator[](size_t idx) {
    LockingPolicy::lock();
    if (idx >= size()) {
      ExceptionPolicy::throw_invalid_index(idx);
    }
    auto& ret = store_.doGet(idx);
    LockingPolicy::unlock();
    return ret;
  }

  virtual const T& operator[](size_t idx) const {
    LockingPolicy::lock();
    if (idx >= size()) {
      ExceptionPolicy::throw_invalid_index(idx);
    }
    auto& ret = store_.doGet(idx);
    LockingPolicy::unlock();
    return ret;
  }

  typedef indexed_iterator<this_t, T> iterator;
  virtual iterator begin() noexcept { return iterator(*this, 0); }
  virtual iterator end() noexcept { return iterator(*this, size()); }

  typedef indexed_iterator<const this_t, const T> const_iterator;
  virtual const_iterator begin() const noexcept {
    return const_iterator(*this, 0);
  }
  virtual const_iterator end() const noexcept {
    return const_iterator(*this, size());
  }
  virtual const_iterator cbegin() const noexcept {
    return const_iterator(*this, 0);
  }
  virtual const_iterator cend() const noexcept {
    return const_iterator(*this, size());
  }

  virtual size_t size() const noexcept { return store_.size(); }
  virtual void resize(size_t n) { resize(n, T()); }
  virtual void resize(size_t n, const T& val) {
    LockingPolicy::lock();
    store_.resize(n, val);
    LockingPolicy::unlock();
  }
  virtual void push_back(const T& val) {
    LockingPolicy::lock();
    store_.resize(store_.size() + 1, val);
    LockingPolicy::unlock();
  }

  virtual void push_back(T&& val) {
    LockingPolicy::lock();
    store_.resize(store_.size() + 1, val);
    LockingPolicy::unlock();
  }

  virtual void pop_back() { resize(size() - 1); }
  virtual void clear() noexcept { resize(0); }
  virtual void flush() {
    LockingPolicy::lock();
    store_.flush();
    LockingPolicy::unlock();
  }

  virtual iterator erase(const_iterator position) {
    LockingPolicy::lock();
    auto pos = begin() + std::distance(cbegin(), position);
    std::copy(position + 1, cend(), pos);
    store_.resize(store_.size() - 1, T());
    LockingPolicy::unlock();
    return pos;
  }

  virtual iterator erase(const_iterator first, const_iterator last) {
    LockingPolicy::lock();
    auto pos = begin() + std::distance(cbegin(), first);
    std::copy(last + 1, cend(), pos);
    store_.resize(store_.size() - std::distance(first, last), T());
    LockingPolicy::unlock();
    return pos;
  }

  // Probably not needed for our use cases, but part of std::vector...
  // virtual iterator insert(const_iterator position, const T& val) {}
  // virtual iterator insert(const_iterator position, size_type n, const T& val)
  // {}
  // template <class InputIterator>
  // virtual iterator insert(const_iterator position, InputIterator first,
  //                         InputIterator last) {}
  // virtual iterator insert(const_iterator position, T&& val) {}
  // virtual iterator insert(const_iterator position, initializer_list<T> il) {}
  // template <class... Args>
  // virtual iterator emplace(const_iterator position, Args&&... args) {}
  // template <class... Args>
  // virtual void emplace_back(Args&&... args) {}

 private:
  StoragePolicy store_;
};
}
}