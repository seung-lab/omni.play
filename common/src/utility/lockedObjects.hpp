#pragma once

#include <list>
#include <map>
#include <set>
#include <utility>
#include <iterator>
#include <unordered_set>
#include <unordered_map>

namespace om {
namespace utility {

template <typename VAL>
class LockedVector {
 public:
  virtual ~LockedVector() {}
  bool empty() {
    zi::guard g(mutex_);
    return vector_.empty();
  }
  void clear() {
    zi::guard g(mutex_);
    vector_.clear();
  }
  void resize(const size_t num) {
    zi::guard g(mutex_);
    return vector_.resize(num);
  }
  VAL& operator[](const size_t i) {
    zi::guard g(mutex_);
    return vector_[i];
  }
  void swap(std::vector<VAL>& vec) {
    zi::guard g(mutex_);
    vector_.swap(vec);
  }
  void push_back(const VAL& v) {
    zi::guard g(mutex_);
    vector_.push_back(v);
  }
  void push_back(VAL&& v) {
    zi::guard g(mutex_);
    vector_.push_back(v);
  }

 private:
  std::vector<VAL> vector_;
  zi::spinlock mutex_;
};

template <typename KEY, typename VAL>
class LockedBoostMap {
 public:
  virtual ~LockedBoostMap() {}
  bool empty() {
    zi::guard g(mutex_);
    return map_.empty();
  }
  void clear() {
    zi::guard g(mutex_);
    map_.clear();
  }
  size_t count(const KEY& key) {
    zi::guard g(mutex_);
    return map_.count(key);
  }

  VAL& operator[](const KEY& key) {
    zi::guard g(mutex_);
    return map_[key];
  }

 private:
  std::unordered_map<KEY, VAL> map_;
  zi::spinlock mutex_;
};

template <typename KEY>
class LockedBoostSet {
 public:
  virtual ~LockedBoostSet() {}
  bool empty() {
    zi::guard g(mutex_);
    return set_.empty();
  }
  void insert(const KEY& key) {
    zi::guard g(mutex_);
    set_.insert(key);
  }
  void erase(const KEY& key) {
    zi::guard g(mutex_);
    set_.erase(key);
  }
  void clear() {
    zi::guard g(mutex_);
    set_.clear();
  }
  bool insertSinceWasntPresent(const KEY& key) {
    zi::guard g(mutex_);
    if (set_.count(key)) {
      return false;
    } else {
      set_.insert(key);
      return true;
    }
  }

 private:
  std::unordered_set<KEY> set_;
  zi::spinlock mutex_;
};

template <typename KEY>
class LockedSet {
 public:
  virtual ~LockedSet() {}
  bool empty() {
    zi::guard g(mutex_);
    return set_.empty();
  }
  void erase(const KEY& key) {
    zi::guard g(mutex_);
    set_.erase(key);
  }
  void insert(const KEY& key) {
    zi::guard g(mutex_);
    set_.insert(key);
  }
  void clear() {
    zi::guard g(mutex_);
    set_.clear();
  }
  bool insertSinceWasntPresent(const KEY& key) {
    zi::guard g(mutex_);
    if (set_.count(key)) {
      return false;
    } else {
      set_.insert(key);
      return true;
    }
  }

 private:
  std::set<KEY> set_;
  zi::spinlock mutex_;
};

template <typename KEY>
class LockedList {
 public:
  virtual ~LockedList() {}
  bool empty() {
    zi::guard g(mutex_);
    return list_.empty();
  }
  void push_back(const KEY& key) {
    zi::guard g(mutex_);
    list_.push_back(key);
  }
  void clear() {
    zi::guard g(mutex_);
    list_.clear();
  }
  void swap(std::list<KEY>& newList) {
    zi::guard g(mutex_);
    list_.swap(newList);
  }

 private:
  std::list<KEY> list_;
  zi::spinlock mutex_;
};

template <typename KEY, typename VAL>
class LockedMultiMap {
 public:
  typedef std::list<VAL> valsCont;

  virtual ~LockedMultiMap() {}

  void insert(const KEY& key, const VAL& val) {
    zi::guard g(mutex_);
    mmap_.insert(std::pair<KEY, VAL>(key, val));
  }
  void clear() {
    zi::guard g(mutex_);
    mmap_.clear();
  }
  std::shared_ptr<valsCont> removeKey(const KEY& key) {
    zi::guard g(mutex_);

    auto vals = std::make_shared<valsCont>();

    auto found = mmap_.equal_range(key);
    for (auto& it : found) {
      vals->push_back(it.second);
    }

    mmap_.erase(key);

    return vals;
  }

 private:
  std::multimap<KEY, VAL> mmap_;
  zi::spinlock mutex_;
};

}  // namespace utility
}  // namespace om
