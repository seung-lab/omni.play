#pragma once

#include "precomp.h"
#include "common/indexedIterator.hpp"
#include "container/policies.hpp"

namespace om {
namespace data {

template <typename T, typename StoragePolicy = VectorBackedStore<T>,
          typename AsyncPolicy = NoAsyncPolicy,
          typename LockingPolicy = NoLockingPolicy,
          typename ExceptionPolicy = ThrowPolicy<size_t, T>>
class dynarray : public LockingPolicy,
                 public ExceptionPolicy,
                 public AsyncPolicy {
 protected:
  typedef dynarray<T, StoragePolicy, AsyncPolicy, LockingPolicy,
                   ExceptionPolicy> this_t;

 public:
  dynarray(StoragePolicy&& store = StoragePolicy()) : store_(store) {}

  virtual T& operator[](size_t idx) {
    lock_guard<this_t> g(*this);
    if (idx >= size()) {
      ExceptionPolicy::throw_invalid_index(idx);
    }
    auto& ret = store_.doGet(idx);
    return ret;
  }

  virtual const T& operator[](size_t idx) const {
    lock_guard<this_t> g(*this);
    if (idx >= size()) {
      ExceptionPolicy::throw_invalid_index(idx);
    }
    auto& ret = store_.doGet(idx);
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
  virtual void resize(size_t n) { store_.resize(n); }

  virtual void clear() noexcept { resize(0); }
  virtual void flush() {
    lock_guard<this_t> g(*this);
    store_.flush();
  }

 private:
  StoragePolicy store_;
};
}
}