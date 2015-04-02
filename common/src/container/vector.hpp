#pragma once

#include "precomp.h"
#include "common/indexedIterator.hpp"
#include "container/policies.hpp"

namespace om {
namespace data {

template <typename T,
          typename StoragePolicy = VectorBackedStore<T>,
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
  virtual void resize(size_t n) { resize(n, T()); }
  virtual void resize(size_t n, const T& val) {
    lock_guard<this_t> g(*this);
    store_.resize(n, val);
  }
  virtual void reserve(size_t n) { store_.reserve(n); }
  virtual void push_back(const T& val) {
    lock_guard<this_t> g(*this);
    store_.resize(store_.size() + 1, val);
  }

  virtual void push_back(T&& val) {
    lock_guard<this_t> g(*this);
    store_.resize(store_.size() + 1, val);
  }

  virtual void pop_back() { resize(size() - 1); }
  virtual void clear() noexcept { resize(0); }
  virtual void flush() {
    lock_guard<this_t> g(*this);
    store_.flush();
  }

  virtual iterator erase(const_iterator position) {
    lock_guard<this_t> g(*this);
    auto pos = begin() + std::distance(cbegin(), position);
    std::copy(position + 1, cend(), pos);
    store_.resize(store_.size() - 1, T());
    return pos;
  }

  virtual iterator erase(const_iterator first, const_iterator last) {
    lock_guard<this_t> g(*this);
    auto pos = begin() + std::distance(cbegin(), first);
    std::copy(last + 1, cend(), pos);
    store_.resize(store_.size() - std::distance(first, last), T());
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
  template <class... Args>
  void emplace_back(Args&&... args) {
    push_back(T(std::forward<Args>(args)...));
  }

 private:
  StoragePolicy store_;
};
}
}
